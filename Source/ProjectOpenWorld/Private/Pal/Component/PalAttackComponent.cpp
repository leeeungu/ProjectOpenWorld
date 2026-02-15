#include "Pal/Component/PalAttackComponent.h"
#include "Pal/Controller/PalAIController.h"
#include "GameBase/BaseCharacter.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameBase/Interface/AttackInterface.h"
#include "GameBase/Animation/BaseAnimInstance.h"
#include "Animation/AnimInstance.h"
#include "Engine/DataAsset.h"
#include "GameFramework/CharacterMovementComponent.h"
//#include "GameBase/Component/CharacterMontageComponent.h"

UPalAttackComponent::UPalAttackComponent() : UActorComponent{}
{
	PrimaryComponentTick.bCanEverTick = false;
	//AttackData.AttackDistance = 50.0f;
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
			Anim->OnMontageQueueEnd.AddUniqueDynamic(this, &UPalAttackComponent::EndAttackMontage);
		}
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

void UPalAttackComponent::EndAttackMontage()
{
	EndAttack();
}

void UPalAttackComponent::ResetAttack()
{
	if (TargetActor)
	{
		TargetActor->OnDestroyed.RemoveDynamic(this, &UPalAttackComponent::TargetIsDead);
	}
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
		//UE_LOG(LogTemp, Warning, TEXT("%s UPalAttackComponent :: already Attacking "), *GetOwner()->GetName());
		return;
	}
	if (IAttackInterface::Execute_IsDead(TargetActor))
	{
		return;
	}
	UBaseAnimInstance* Anim = Cast< UBaseAnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance());
	if (Anim)
	{
		Anim->ChangeMontageArray(AttackData.AttackData);
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
	UE_LOG(LogTemp, Warning, TEXT("%s UPalAttackComponent :: StartAttack Called "), *GetOwner()->GetName());
}

void  UPalAttackComponent::EndAttack()
{
	if (!bAttacking)
		return;

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
	UE_LOG(LogTemp, Warning, TEXT("%s UPalAttackComponent :: EndAttack Finished "), *GetOwner()->GetName());
	TargetActor = nullptr;
	AttackData.AttackData.Empty();
	AttackData.AttackSlot = ESubAttackType::None_AttackType;
}

void UPalAttackComponent::StopAttack()
{
	UBaseAnimInstance* Anim = Cast< UBaseAnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance());
	if(Anim)
	{
		Anim->StopMontageQueue();
	}
	EndAttack();
//	UE_LOG(LogTemp, Warning, TEXT("%s UPalAttackComponent :: StopAttack Called "), *GetOwner()->GetName());
	ResetAttackData();
}

bool UPalAttackComponent::TargetIsInRange() const
{
	if (TargetActor && OwnerCharacter)
	{
		const double Distance = FVector::DistSquared(OwnerCharacter->GetActorLocation(), TargetActor->GetActorLocation());
		if (Distance <= AttackData.AttackDistance * AttackData.AttackDistance)
		{
			return true;
		}
	}
	return false;
}
