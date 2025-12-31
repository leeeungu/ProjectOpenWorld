#include "Pal/Component/PalAttackComponent.h"
#include "Pal/Controller/PalAIController.h"
#include "GameBase/BaseCharacter.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameBase/Interface/AttackInterface.h"
#include "GameBase/MetaData/AnimMetaData_LoopData.h"
#include "Animation/AnimInstance.h"

UPalAttackComponent::UPalAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
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

	if (Controller)
	{
		//Controller->ReceiveMoveCompleted.AddDynamic(this, &UPalAttackComponent::FinishMove);
		OwnerCharacter->GetMesh()->GetAnimInstance()->OnMontageBlendingOut.AddDynamic(this, &UPalAttackComponent::PlayNextAttack);
	}
}

void UPalAttackComponent::TargetIsDead(AActor* Actor)
{
	EndAttack();
}

void UPalAttackComponent::PlayNextAttack(UAnimMontage* Montage, bool bInterrupted)
{
	AttackIndex++;
	if (AttackData.AttackData.IsValidIndex(AttackIndex))
	{
		Curent = AttackData.AttackData[AttackIndex];
		OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(Curent);
		float Length = Curent->GetPlayLength();
		UAnimMetaData_LoopData* Loop = Curent->FindMetaDataByClass<UAnimMetaData_LoopData>();
		if (Loop)
		{
			Length = Length * Loop->GetLoopCount();
		}
		ChangeAnim = true;
		AttackTime += Length;
		return;
	}
	else
	{
		EndAttack();
	}
}

void UPalAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//Timer.UpdateTimer(DeltaTime);
	//if (Timer.IsFinished())
	//{
	//	EndAttack();
	//	return;
	//}
	//float Current = Timer.GetCurrentTime();

	//if (AttackTime <= Current + 0.1f)
	//{
	//	AttackIndex++;
	//	if (AttackData.AttackData.IsValidIndex(AttackIndex))
	//	{
	//		Curent = AttackData.AttackData[AttackIndex];
	//		OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(Curent);
	//		float Length = Curent->GetPlayLength();
	//		UAnimMetaData_LoopData* Loop = Curent->FindMetaDataByClass<UAnimMetaData_LoopData>();
	//		if (Loop)
	//		{
	//			Length = Length * Loop->GetLoopCount();
	//		}
	//		ChangeAnim = true;
	//		AttackTime += Length;
	//		return;
	//	}
	//	else
	//	{
	//		EndAttack();
	//	}
	//}
	//ChangeAnim = false;
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

	//Current = Default;
	//if(AttackData.AttackSlot == ESubAttackType::Skill01)
	//	Current = Skill01;
}

void UPalAttackComponent::StartAttack()
{
	if (!bSetTargetData)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s UPalAttackComponent :: SetAttackData before use StartAttack "), *GetOwner()->GetName());
		EndAttack();
		return;
	}
	float Total{};
	for (const TObjectPtr<UAnimMontage>& Anim : AttackData.AttackData)
	{
		float Length = Anim->GetPlayLength();
		UAnimMetaData_LoopData* Loop = Anim->FindMetaDataByClass<UAnimMetaData_LoopData>();
		if (Loop)
		{
			Length = Length * Loop->GetLoopCount();
		}
		Total += Length;
	}
	if (AttackData.AttackData.Num() > 0)
		Curent = AttackData.AttackData[0];
	OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(Curent);
	Timer.InitTimer(Total);
	bAttacking = true;
	Controller->SetFocus(AttackData.TargetActor);
	Timer.StartTimer();
	AttackTime = 0;
	AttackIndex = 0;
	if (OnPalAttackStart.IsBound())
	{
		OnPalAttackStart.Broadcast();
	}
	SetComponentTickEnabled(true);
}

void  UPalAttackComponent::EndAttack()
{
	//UE_LOG(LogTemp, Warning, TEXT("UPalAttackComponent :: EndAttack"));
	SetComponentTickEnabled(false);
	Timer.StopTimer();
	bSetTargetData = false;
	bAttacking = false;
	//bMoveStarted = false;
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
	Curent = nullptr;
}
