#include "Pal/Component/PalAttackComponent.h"
#include "Pal/Controller/PalAIController.h"
#include "GameBase/BaseCharacter.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameBase/Interface/AttackInterface.h"
#include "GameBase/Animation/BaseAnimInstance.h"
#include "Animation/AnimInstance.h"
#include "Engine/DataAsset.h"
#include "GameFramework/CharacterMovementComponent.h"

UPalAttackComponent::UPalAttackComponent() : UActorComponent{}
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UPalAttackComponent::BeginPlay()
{
	Super::BeginPlay();
	if (OwnerCharacter = Cast<ABaseCharacter>(GetOwner()))
	{
		Controller = Cast< APalAIController>(OwnerCharacter->GetController());
		UBaseAnimInstance* Anim = Cast< UBaseAnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance());
		if (Anim)
		{
			Anim->OnMontageQueueEnd.AddUniqueDynamic(this, &UPalAttackComponent::EndAttack);
		}
	}
	if (!Controller)
	{
		GetOwner()->Destroy();
		return;
	}
}

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
	if (AttackDataAsset && AllAttackDataArray.IsEmpty())
	{
		AttackDataAsset->GetAllRows("", AllAttackDataArray);
		uint8 Index = static_cast<uint8>(eType);
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
	AttackData.AttackSlot = eType;
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

		UBaseAnimInstance* Anim = Cast< UBaseAnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance());
		if (Anim)
		{
			Anim->ChangeMontageArray(AttackData.AttackData);
			Anim->PlayMontageQueue();
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
	UBaseAnimInstance* Anim = Cast< UBaseAnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance());
	if(Anim)
	{
		Anim->StopMontageQueue();
	}
}

bool UPalAttackComponent::TargetIsInRange() const
{
	if (TargetActor.IsValid() && OwnerCharacter)
	{
		const double Distance = FVector::DistSquared(OwnerCharacter->GetActorLocation(), TargetActor->GetActorLocation());
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