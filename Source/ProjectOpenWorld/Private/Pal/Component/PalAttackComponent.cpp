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
	//AttackData.AttackDistance = 50.0f;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetComponentTickEnabled(false);
}

void UPalAttackComponent::BeginPlay()
{
	Super::BeginPlay();
	if (OwnerCharacter = Cast<ABaseCharacter>(GetOwner()))
	{
		Controller = Cast< APalAIController>(OwnerCharacter->GetController());
	}
	if (!Controller)
	{
		GetOwner()->Destroy();
		return;
	}
}

void UPalAttackComponent::TargetIsDead(AActor* Actor)
{
	EndAttack();
}

void UPalAttackComponent::ResetAttackData()
{
	bSetAttackData = false;
	bAttacking = false;
	AttackIndex = 0;
}

void UPalAttackComponent::SetAttackTarget(AActor* Actor)
{
	TargetActor = Actor;
	if (!TargetActor || TargetActor->IsPendingKillPending() || bAttacking)
		return;
	if (!TargetActor->Implements<UAttackInterface>())
	{
		UE_LOG(LogTemp, Warning, TEXT("%s UPalAttackComponent :: TargetActor has not interface"), *TargetActor.Get()->GetName());
		return;
	}
	TargetActor->OnDestroyed.AddUniqueDynamic(this, &UPalAttackComponent::TargetIsDead);
	bSetTargetData = true;
}

void UPalAttackComponent::SetAttackData(ESubAttackType eType)
{
	if (!AttackDataAsset && AttackData.AttackData.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("%s UPalAttackComponent :: SetAttackData no AttackDataAsset "), *GetOwner()->GetName());
		AttackData.AttackSlot = ESubAttackType::None_AttackType;
		AttackIndex = 0;
		bSetAttackData = false;
		return;
	}
	bSetAttackData = false;
	if (AttackDataAsset)
	{
		TArray<FPalAttackDataTable*> ArrayData{};

		AttackDataAsset->GetAllRows("", ArrayData);
		uint8 Index = static_cast<uint8>(eType);
		UE_LOG(LogTemp, Warning, TEXT("%s UPalAttackComponent :: SetAttackData in DataTable %d"), *GetOwner()->GetName(), Index);
		if (ArrayData.IsValidIndex(Index))
		{
			AttackData.AttackData = ArrayData[Index]->AttackData;
			AttackData.AttackDistance = ArrayData[Index]->AttackDistance;
			bSetAttackData = true;
		}
		else
		{
			eType = ESubAttackType::None_AttackType;
			UE_LOG(LogTemp, Warning, TEXT("%s UPalAttackComponent :: SetAttackData no data in DataTable "), *GetOwner()->GetName());
		}
	}
	AttackData.AttackSlot = eType;
	AttackIndex = 0;
}

void UPalAttackComponent::StartAttack()
{
	if (!bSetTargetData)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s UPalAttackComponent :: SetAttackData before use StartAttack "), *GetOwner()->GetName());
		EndAttack();
		return;
	}
	if (bAttacking)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s UPalAttackComponent :: already Attacking "), *GetOwner()->GetName());
		return;
	}
	UBaseAnimInstance* Anim = Cast< UBaseAnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance());
	if (Anim)
	{
		Anim->SetMontageQueueInterface(this);
		Anim->PlayMontageQueue();
	}
	bAttacking = true;
	Controller->SetFocus(TargetActor);
	OwnerCharacter->GetCharacterMovement()->StopMovementImmediately();
	OwnerCharacter->GetCharacterMovement()->DisableMovement();
	AttackIndex = 0;
	if (OnPalAttackStart.IsBound())
	{
		OnPalAttackStart.Broadcast();
	}
	SetComponentTickEnabled(true);
}

void  UPalAttackComponent::EndAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("%s UPalAttackComponent :: EndAttack "), *GetOwner()->GetName());
	SetComponentTickEnabled(false);
	ResetAttackData();
	bSetTargetData= false;
	Controller->SetFocus(nullptr);
	if (TargetActor)
	{
		TargetActor->OnDestroyed.RemoveDynamic(this, &UPalAttackComponent::TargetIsDead);
	}
	if (OnPalAttackEnd.IsBound())
	{
		OnPalAttackEnd.Broadcast();
	}
	TargetActor = nullptr;
}

void UPalAttackComponent::StopAttack()
{
	ResetAttackData();
	OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_Stop(0.01f);
}

bool UPalAttackComponent::TargetIsInRange() const
{
	if (TargetActor && OwnerCharacter)
	{
		const float Distance = FVector::DistSquared(OwnerCharacter->GetActorLocation(), TargetActor->GetActorLocation());
		UE_LOG(LogTemp, Warning, TEXT("%s UPalAttackComponent :: TargetIsInRange Distance : %f , AttackDistance : %f"), *GetOwner()->GetName(), Distance, AttackData.AttackDistance* AttackData.AttackDistance);
		if (Distance <= AttackData.AttackDistance * AttackData.AttackDistance)
		{
			return true;
		}
	}
	return false;
}

UAnimMontage* UPalAttackComponent::GetMontage() const
{
	UAnimMontage* NextMontage = nullptr;
	if (AttackData.AttackData.IsValidIndex(AttackIndex) && bSetAttackData)
	{
		NextMontage = AttackData.AttackData[AttackIndex];
	}
	return NextMontage;
}

void UPalAttackComponent::MontageStartEvent(UBaseAnimInstance* BaseAnim, UAnimMontage* Montage)
{
	//UE_LOG(LogTemp, Warning, TEXT("MontageStartEvent"));
}

void UPalAttackComponent::MontageBlendingEvent(UBaseAnimInstance* BaseAnim, UAnimMontage* Montage, bool bInterrupted)
{
	if (!Montage || !BaseAnim)
		return;
	if (!BaseAnim->IsLoop())
		AttackIndex++;
	if (!AttackData.AttackData.IsValidIndex(AttackIndex))
	{
		BaseAnim->SetMontageQueueInterface(nullptr);
		UE_LOG(LogTemp, Warning, TEXT("%s UPalAttackComponent :: MontageBlendingEvent End Attack "), *GetOwner()->GetName());
		ResetAttackData();
		return;
	}
	BaseAnim->PlayMontageQueue();
}