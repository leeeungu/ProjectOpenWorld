#include "Pal/Component/PalAttackComponent.h"
#include "Pal/Controller/PalAIController.h"
#include "GameFramework/Character.h"
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
	if (ACharacter* Pawn = Cast<ACharacter>(GetOwner()))
	{
		Controller = Cast< APalAIController>(Pawn->GetController());
	//	Pawn->GetMesh()->GetAnimInstance()->getanim;
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
	if (bMoveStarted == false && bSetTargetData)
	{
		AttackRange = AttackDistance * AttackDistance;
		HalfAttackAngle = abs(AttackAngle * 0.5f);
		double DisSquer = FVector::DistSquared(AttackData.TargetActor->GetActorLocation(), GetOwner()->GetActorLocation());

		if (DisSquer > AttackRange)
		{
			bMoveStarted = true;
			bAttacking = false;
			EPathFollowingRequestResult::Type PathResult = Controller->MoveToActor(AttackData.TargetActor, AttackDistance);
			if (PathResult == EPathFollowingRequestResult::Failed)
			{
				EndAttack();
				return;
			}
		}

		float Angle = GetTargetRotationYaw();

		if (-HalfAttackAngle > Angle || Angle > HalfAttackAngle)
		{
			float Yaw = DeltaTime * 15.0f;
			bAttacking = false;
			if (Angle < 0)
				Yaw = -Yaw;
			FRotator Roate = { 0,Yaw ,0 };
			GetOwner()->AddActorWorldRotation(Roate);
			UE_LOG(LogTemp, Log, TEXT("True"));
			return;
		}
		bAttacking = true;
	}
	//	//<= AttackDistance * AttackDistance;
	//	bAttacking = FVector::DistSquared(AttackData.TargetActor->GetActorLocation(), GetOwner()->GetActorLocation()) <= AttackDistance * AttackDistance;
	//	//float AttackRange = 5.0f;
	//	bAttacking = bAttacking && (Angle <= AttackRange) && Angle >= 0;
	//	FMath::
	//	if (bMoveStarted == false && bAttacking == false)
	//	{
	//		bMoveStarted = true;
	//		bAttacking = false;

	//		if (IAttackInterface::Execute_IsDead(AttackData.TargetActor))
	//		{
	//			EndAttack();
	//		}
	//		else
	//		{
	//			EPathFollowingRequestResult::Type PathResult = Controller->MoveToActor(AttackData.TargetActor, AttackDistance);
	//			if (Angle > AttackRange || Angle < 0)
	//			{
	//				if (AttackData.TargetActor && AttackData.TargetActor.Get())
	//				{
	//					FRotator Roate = { 0,DeltaTime * 2.0f * AttackRange ,0 };
	//					//Roate.Yaw = FMath::Clamp(Roate.Yaw, -AttackRange * 2, AttackRange * 2);
	//					GetOwner()->AddActorWorldRotation(Roate);
	//					UE_LOG(LogTemp, Log, TEXT("True"));
	//				}
	//			}
	//			else
	//			{
	//				if (PathResult == EPathFollowingRequestResult::AlreadyAtGoal)
	//				{
	//					bMoveStarted = false;
	//					bAttacking = true;
	//					bCanRotate = true;
	//				}
	//				else if (PathResult == EPathFollowingRequestResult::Failed)
	//				{
	//					EndAttack();
	//				}
	//				else if (PathResult == EPathFollowingRequestResult::RequestSuccessful)
	//				{
	//					bCanRotate = true;
	//				}
	//			}
	//		}
	//	}
	//}
	//else if (!AttackData.TargetActor || !AttackData.TargetActor.Get())
	//{
	//	//EndAttack();
	//	//UE_LOG(LogTemp, Warning, TEXT("%s UPalAttackComponent :: NoTargetActor"), * GetOwner()->GetName());
	//}
	//else
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
	if (bMoveStarted == false || Result != EPathFollowingResult::Type::Success)
	{
		EndAttack();
		return;
	}
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
