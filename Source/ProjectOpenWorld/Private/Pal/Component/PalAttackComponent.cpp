#include "Pal/Component/PalAttackComponent.h"
#include "Pal/Controller/PalAIController.h"
#include "GameBase/BaseCharacter.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameBase/Interface/AttackInterface.h"

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
		Controller->ReceiveMoveCompleted.AddDynamic(this, &UPalAttackComponent::FinishMove);
	}
}

void UPalAttackComponent::TargetIsDead(AActor* Actor)
{
	EndAttack();
}

void UPalAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bMoveStarted == false && bSetTargetData && OwnerCharacter)
	{
		AttackRange = AttackDistance * AttackDistance;
		HalfAttackAngle = abs(AttackAngle * 0.5f);
		double DisSquer = FVector::DistSquared(AttackData.TargetActor->GetActorLocation(), GetOwner()->GetActorLocation());
		if (DisSquer > AttackRange)
		{
			bMoveStarted = true;
			bAttacking = false;
			bCanRotate = false;
			//EPathFollowingRequestResult::Type PathResult = Controller->MoveToActor(AttackData.TargetActor, AttackDistance);
			////UE_LOG(LogTemp, Warning, TEXT("Move To Target"));
			//if (PathResult == EPathFollowingRequestResult::Failed)
			//{
			//	UE_LOG(LogTemp, Warning, TEXT("Move Failed"));
			//	EndAttack();
			//	return;
			//}
			//else if (PathResult == EPathFollowingRequestResult::RequestSuccessful)
			//	return;
		}

		//if (!bCanRotate)
		//{
		//	float Angle = GetTargetRotationYaw();
		//	if (-HalfAttackAngle > Angle || Angle > HalfAttackAngle)
		//	{
		//		bCanRotate = true;
		//		bAttacking = false;
		//		return;
		//	}
		//}
		bAttacking = true;
		bCanRotate = false;
	}
}

void UPalAttackComponent::SetAttackData(FPalAttackData sData)
{
	if (!sData.TargetActor || sData.TargetActor->IsPendingKillPending())
		return;
	if (!sData.TargetActor->Implements<UAttackInterface>())
	{
		UE_LOG(LogTemp, Warning, TEXT("%s UPalAttackComponent :: TargetActor has not interface"), *sData.TargetActor.Get()->GetName());
		return;
	}
	sData.TargetActor->OnDestroyed.AddUniqueDynamic(this, &UPalAttackComponent::TargetIsDead);
	bSetTargetData = true;
	AttackData = sData;
	Current = Default;
	if(AttackData.AttackSlot == ESubAttackType::Skill01)
		Current = Skill01;
}

void UPalAttackComponent::StartAttack()
{
	if (!bSetTargetData)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s UPalAttackComponent :: SetAttackData before use StartAttack "), *GetOwner()->GetName());
		EndAttack();
		return;
	}
	Controller->SetFocus(AttackData.TargetActor);
	SetComponentTickEnabled(true);
	if (OnPalAttackStart.IsBound())
	{
		OnPalAttackStart.Broadcast();
	}
}

void  UPalAttackComponent::EndAttack()
{
	//UE_LOG(LogTemp, Warning, TEXT("UPalAttackComponent :: EndAttack"));
	SetComponentTickEnabled(false);
	bSetTargetData = false;
	bAttacking = false;
	bMoveStarted = false;
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

void UPalAttackComponent::FinishMove(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (bMoveStarted == false || Result == EPathFollowingResult::Type::Invalid 
		|| Result == EPathFollowingResult::Type::Aborted)
		//|| Result == EPathFollowingResult::Type::Aborted)
	{
		
		EndAttack();
		return;
	}
	//switch (Result)
	//{
	//case EPathFollowingResult::Success:
	//	UE_LOG(LogTemp, Warning, TEXT("UPalAttackComponent :: FinishMove Success"));
	//	break;
	//case EPathFollowingResult::Blocked:
	//	UE_LOG(LogTemp, Warning, TEXT("UPalAttackComponent :: FinishMove Blocked"));
	//	break;
	//case EPathFollowingResult::OffPath:
	//	UE_LOG(LogTemp, Warning, TEXT("UPalAttackComponent :: FinishMove OffPath"));
	//	break;
	//case EPathFollowingResult::Skipped_DEPRECATED:
	//	UE_LOG(LogTemp, Warning, TEXT("UPalAttackComponent :: FinishMove Skipped"));
	//	break;
	////case EPathFollowingResult::Aborted:
	////	UE_LOG(LogTemp, Warning, TEXT("UPalAttackComponent :: FinishMove Aborted"));
	////	break;
	////case EPathFollowingResult::Invalid:
	////	UE_LOG(LogTemp, Warning, TEXT("UPalAttackComponent :: FinishMove Invalid"));
	////	return;
	////	break;
	//default:
	//	break;
	//}
	bMoveStarted = false;
}

float UPalAttackComponent::GetTargetRotationYaw() const
{
	float dot = FVector::DotProduct(
		(AttackData.TargetActor->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal2D(),
		GetOwner()->GetActorForwardVector().GetSafeNormal2D());
	float Angle = FMath::RadiansToDegrees(FMath::Acos(dot));
	float Direction = FVector::DotProduct(
		(AttackData.TargetActor->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal2D(),
		GetOwner()->GetActorRightVector().GetSafeNormal2D());
	Direction = Direction > 0 ? 1.0 : -1.0f;
	return Angle  * Direction;
}


//DrawDebugLine(OwnerCharacter->GetWorld(),
	//	OwnerCharacter->GetActorLocation(), 
	//	OwnerCharacter->GetActorLocation() + OwnerCharacter->GetActorForwardVector() * 100.0f,
	//	FColor::Red, false , 1.0f);
	//DrawDebugLine(OwnerCharacter->GetWorld(),
	//	OwnerCharacter->GetActorLocation(), 
	//	OwnerCharacter->GetActorLocation() + (OwnerCharacter->GetActorForwardVector().RotateAngleAxis(OwnerCharacter->GetTurnAngle(), FVector{ 0,0,1 }))* 100.0f,
	//	FColor::Green, false, 1.0f);