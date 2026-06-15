#include "Pal/Component/PalAttackComponent.h"
#include "Pal/Controller/PalAIController.h"
#include "GameFramework/Character.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameBase/Interface/AttackInterface.h"
#include "GameBase/Animation/BaseAnimInstance.h"
#include "Animation/AnimInstance.h"
#include "Engine/DataAsset.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Pal/Character/PalBaseCharacter.h"
#include "Pal/Subsystem/PalCharacterDataSubsystem.h"
#include "Pal/DataTable/PalMonsterData.h"


UPalAttackComponent::UPalAttackComponent() : UActorComponent{}
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UPalAttackComponent::BeginPlay()
{
	Super::BeginPlay();
	SetAttackDataTableFromSubsystem();
	if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
	{
		OwnerAnimInstance = Cast< UBaseAnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance());
		Controller = Cast< APalAIController>(OwnerCharacter->GetController());
		if (OwnerAnimInstance)
		{
			OwnerAnimInstance->OnMontageQueueEnd.AddUniqueDynamic(this, &UPalAttackComponent::EndAttack);
		}
	}
	//if (!Controller)
	//{
	//	GetOwner()->Destroy();
	//	return;
	//}
}

FName UPalAttackComponent::ResolveOwnerPalName() const
{
	// creature/monster 공통: PalBaseCharacter 기준 (V1 owner면 nullptr → 스킵)
	if (const APalBaseCharacter* PalChar = Cast<APalBaseCharacter>(GetOwner()))
		return PalChar->GetPalName();
	return NAME_None;
}

void UPalAttackComponent::SetAttackDataTableFromSubsystem()
{
	const FName PalName = ResolveOwnerPalName();
	if (PalName.IsNone())
		return;   // PalBaseCharacter 아님(V1 등) → 기존 AttackDataAsset 유지

	const FPalMonsterData* Row = nullptr;
	if (UPalCharacterDataSubsystem::GetPalMonsterData(PalName, Row) && Row && Row->AttackDataTable)
	{
		AttackDataAsset = Row->AttackDataTable;
		AllAttackDataArray.Reset();   // 새 테이블 반영 위해 lazy 캐시 무효화
	}
	else
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[PalAttack] '%s' AttackDataTable 미발견 — 기존 AttackDataAsset 유지"),
			*PalName.ToString());
	}
}

void UPalAttackComponent::OnRegister()
{
	Super::OnRegister();
#if WITH_EDITOR
	const UWorld* World = GetWorld();
	if (World && !World->IsGameWorld())   // 에디터 프리뷰 컨텍스트에서만
		ValidateAttackDataTableInEditor();
#endif
}

#if WITH_EDITOR
void UPalAttackComponent::ValidateAttackDataTableInEditor()
{
	const FName PalName = ResolveOwnerPalName();
	if (PalName.IsNone())
		return;

	const FPalMonsterData* Row = nullptr;
	UPalCharacterDataSubsystem::GetPalMonsterData(PalName, Row);
	if (!Row || !Row->AttackDataTable)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[PalAttack] '%s' 의 AttackDataTable이 통합 MonsterDT에 없습니다. (Owner: %s)"),
			*PalName.ToString(),
			GetOwner() ? *GetOwner()->GetName() : TEXT("None"));
	}
}
#endif

void UPalAttackComponent::ResetAttackData()
{
	bSetAttackData = false;
	bAttacking = false;
}

void UPalAttackComponent::ResetAttack()
{
	TargetActor = nullptr;
	AttackData.AttackData.Empty();
	AttackData.AttackSlot = ESubAttackType::None_AttackType;
	ResetAttackData();
}

void UPalAttackComponent::SetAttackTarget(AActor* Actor)
{
	if (!Actor || Actor->IsPendingKillPending() || bAttacking)
		return;
	if (!Actor->Implements<UAttackInterface>())
	{
		UE_LOG(LogTemp, Warning, TEXT("%s UPalAttackComponent :: TargetActor has not interface"), *TargetActor.Get()->GetName());
		return;
	}
	TargetActor = Actor;
	bSetTargetData = true;
}

void UPalAttackComponent::SetAttackData(ESubAttackType eType)
{
	if (!AttackDataAsset && AttackData.AttackData.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("%s UPalAttackComponent :: SetAttackData no AttackDataAsset "), *GetOwner()->GetName());
		AttackData.AttackSlot = ESubAttackType::None_AttackType;
		bSetAttackData = false;
		return;
	}
	bSetAttackData = false;
	uint8 Index = static_cast<uint8>(eType);
	if (AttackDataAsset && AllAttackDataArray.IsEmpty())
	{
		AttackDataAsset->GetAllRows("", AllAttackDataArray);
		CoolDownArray.Init(false, static_cast<uint8>(ESubAttackType::Max_AttackType));
		if (AllAttackDataArray.IsValidIndex(Index))
		{
			AttackData.AttackData = AllAttackDataArray[Index]->AttackData;
			AttackData.AttackDistance = AllAttackDataArray[Index]->AttackDistance;
			bSetAttackData = true;
		}
		else
		{
			eType = ESubAttackType::None_AttackType;
			UE_LOG(LogTemp, Warning, TEXT("%s UPalAttackComponent :: SetAttackData no data in DataTable "), *GetOwner()->GetName());
		}
	}
	if(AllAttackDataArray.IsValidIndex(Index))
		AttackData = *AllAttackDataArray[Index];
	else
		AttackData = FPalAttackDataTable{};
}

void UPalAttackComponent::StartAttack()
{
	if (!bSetTargetData)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s UPalAttackComponent :: SetAttackData before use StartAttack "), *GetOwner()->GetName());
		ResetAttack();
		return;
	}
	if (!TargetActor.IsValid() || IAttackInterface::Execute_IsDead(TargetActor.Get()))
	{
		ResetAttack();
		return;
	}
	uint8 Index = static_cast<uint8>(AttackData.AttackSlot);
	if (bAttacking )
		return;

	if (CoolDownArray.IsValidIndex(Index) && CoolDownArray[Index])
	{
		FTimerHandle Handle{};
		GetWorld()->GetTimerManager().SetTimer(Handle,
			[this]()
			{
				StartAttack();
			},
			2, false, 2);
	}
	else
	{
		CoolDownArray[Index] = true;

		if (OwnerAnimInstance)
		{
			OwnerAnimInstance->ChangeMontageArray(AttackData.AttackData);
			OwnerAnimInstance->PlayMontageQueue();
		}
		bAttacking = true;
		FTimerHandle Handle{};
		UE_LOG(LogTemp, Log, TEXT("%s UPalAttackComponent :: CoolDown Start"), *GetOwner()->GetName());
		GetWorld()->GetTimerManager().SetTimer(Handle,
			[this, Index]()
			{
				UE_LOG(LogTemp, Log, TEXT("%s UPalAttackComponent :: CoolDown End"), *GetOwner()->GetName());
				if (CoolDownArray.IsValidIndex(Index))
				{
					CoolDownArray[Index] = false;
				}
			},
			AttackData.AttackCooldown, false, AttackData.AttackCooldown);
		if (OnPalAttackStart.IsBound())
		{
			OnPalAttackStart.Broadcast();
		}
	}
}

void  UPalAttackComponent::EndAttack()
{
	if (!bAttacking)
		return;
	ResetAttackData();
	bSetTargetData= false;
	if (OnPalAttackEnd.IsBound())
	{
		OnPalAttackEnd.Broadcast();
	}
}

void UPalAttackComponent::StopAttack()
{
	if(OwnerAnimInstance)
	{
		OwnerAnimInstance->StopMontageQueue();
	}
}

bool UPalAttackComponent::TargetIsInRange() const
{
	if (TargetActor.IsValid() )
	{
		const double Distance = FVector::DistSquared(GetOwner()->GetActorLocation(), TargetActor->GetActorLocation());
		UE_LOG(LogTemp, Log, TEXT("%s UPalAttackComponent :: TargetIsInRange Distance : %f"), *GetOwner()->GetName(), Distance);
		return Distance <= AttackData.AttackDistance * AttackData.AttackDistance;
	}
	return false;
}

bool UPalAttackComponent::IsTargetNotDead() const
{
	if (TargetActor.IsValid())
	{
		return !IAttackInterface::Execute_IsDead(TargetActor.Get());
	}
	return false;
}
bool UPalAttackComponent::IsCoolDown(ESubAttackType Type) const
{
	uint8 Index = static_cast<uint8>(Type);
	if (CoolDownArray.IsValidIndex(Index))
	{
		UE_LOG(LogTemp, Log, TEXT("%s UPalAttackComponent :: IsCoolDown %d"), *GetOwner()->GetName(), CoolDownArray[Index]);
		return !CoolDownArray[Index];
	}
	return false;
}
AActor* UPalAttackComponent::GetTargetActor() const 
{
	return TargetActor.IsValid() ? TargetActor.Get() : nullptr;
}