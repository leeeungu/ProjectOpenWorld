#include "Pal/Component/PalAttackComponent.h"
#include "Pal/Controller/PalAIController.h"
#include "GameBase/BaseCharacter.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameBase/Interface/AttackInterface.h"
#include "GameBase/Animation/BaseAnimInstance.h"
#include "Animation/AnimInstance.h"

UPalAttackComponent::UPalAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	AttackDistance = 50.0f;
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

void UPalAttackComponent::SetAttackData(FPalAttackData sData)
{
	if (!sData.TargetActor || sData.TargetActor->IsPendingKillPending() || bAttacking)
		return;
	if (!sData.TargetActor->Implements<UAttackInterface>())
	{
		UE_LOG(LogTemp, Warning, TEXT("%s UPalAttackComponent :: TargetActor has not interface"), *sData.TargetActor.Get()->GetName());
		return;
	}
	sData.TargetActor->OnDestroyed.AddUniqueDynamic(this, &UPalAttackComponent::TargetIsDead);
	bSetTargetData = true;
	AttackData.AttackSlot = sData.AttackSlot;
	AttackData.TargetActor = sData.TargetActor;
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

	UBaseAnimInstance* Anim = Cast< UBaseAnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance());
	if (Anim)
	{
		Anim->SetMontageQueueInterface(this);
		Anim->PlayMontageQueue();
	}
	bAttacking = true;
	Controller->SetFocus(AttackData.TargetActor);
	AttackIndex = 0;
	if (OnPalAttackStart.IsBound())
	{
		OnPalAttackStart.Broadcast();
	}
	SetComponentTickEnabled(true);
}

void  UPalAttackComponent::EndAttack()
{
	SetComponentTickEnabled(false);
	bSetTargetData = false;
	bAttacking = false;
	Controller->SetFocus(nullptr);
	if (AttackData.TargetActor)
	{
		AttackData.TargetActor->OnDestroyed.RemoveDynamic(this, &UPalAttackComponent::TargetIsDead);
	}
	if (OnPalAttackEnd.IsBound())
	{
		OnPalAttackEnd.Broadcast();
	}
	AttackData.TargetActor = nullptr;
}

UAnimMontage* UPalAttackComponent::GetMontage() const
{
	UAnimMontage* NextMontage = nullptr;
	if (AttackData.AttackData.IsValidIndex(AttackIndex))
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
		EndAttack();
		return;
	}
	BaseAnim->PlayMontageQueue();
}