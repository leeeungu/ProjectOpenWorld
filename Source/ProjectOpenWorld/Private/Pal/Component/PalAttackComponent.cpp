#include "Pal/Component/PalAttackComponent.h"
#include "Pal/Controller/PalAIController.h"
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
	if (APawn* Pawn = Cast<APawn>(GetOwner()))
		Controller = Cast< APalAIController>(Pawn->GetController());
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
	if (bCanAttack && AttackData.TargetActor && AttackData.TargetActor.Get())
	{
		bAttacking = FVector::DistSquared(AttackData.TargetActor->GetActorLocation(), GetOwner()->GetActorLocation()) <= AttackDistance * AttackDistance;
		float dot = FVector::DotProduct(
			(AttackData.TargetActor->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal2D(),
			GetOwner()->GetActorForwardVector().GetSafeNormal2D());
		float Angle = FMath::RadiansToDegrees(FMath::Acos(dot));
		float AttackRange = 5.0f;
		bAttacking = bAttacking && (Angle <= AttackRange) && Angle >= 0;
		
		if (bMoveStarted == false && bAttacking == false)
		{
			bMoveStarted = true;
			bAttacking = false;

			if (IAttackInterface::Execute_IsDead(AttackData.TargetActor))
			{
				EndAttack();
			}
			else
			{
				EPathFollowingRequestResult::Type PathResult = Controller->MoveToActor(AttackData.TargetActor, AttackDistance);
				if (Angle > AttackRange || Angle < 0 )
				{
					float Direction = FVector::DotProduct(
						(AttackData.TargetActor->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal2D(),
						GetOwner()->GetActorRightVector().GetSafeNormal2D());
					Direction = Direction > 0 ? 1.0 : -1.0f;

					FRotator Roate = { 0,DeltaTime * 600.0f * Direction ,0 };
					//Roate.Yaw = FMath::Clamp(Roate.Yaw, -AttackRange * 2, AttackRange * 2);
					GetOwner()->AddActorWorldRotation(Roate);
				}
				else
				{
					if (PathResult == EPathFollowingRequestResult::AlreadyAtGoal)
					{
						bMoveStarted = false;
						bAttacking = true;
					}
					else if (PathResult == EPathFollowingRequestResult::Failed)
					{
						EndAttack();
					}
				}
			}
		}
	}
	else if (!AttackData.TargetActor || !AttackData.TargetActor.Get())
	{
		//EndAttack();
		//UE_LOG(LogTemp, Warning, TEXT("%s UPalAttackComponent :: NoTargetActor"), * GetOwner()->GetName());
	}
	//else
}

void UPalAttackComponent::SetAttackData(FPalAttackData sData)
{
	if (!sData.TargetActor || sData.TargetActor->IsPendingKillPending())
		return;
	if (!sData.TargetActor->Implements<UAttackInterface>())
		return;
	sData.TargetActor->OnDestroyed.AddUniqueDynamic(this, &UPalAttackComponent::TargetIsDead);
	bCanAttack = true;
	AttackData = sData;
	bAttacking = false;
	Current = Default;
	if(AttackData.AttackSlot == ESubAttackType::Skill01)
		Current = Skill01;
}

void UPalAttackComponent::StartAttack()
{
	if (!bCanAttack)
	{
		EndAttack();
		return;
	}
	//UE_LOG(LogTemp, Warning, TEXT("UPalAttackComponent :: StartAttack"));
	EPathFollowingRequestResult::Type Result = Controller->MoveToActor(AttackData.TargetActor, AttackDistance);
	if (Result == EPathFollowingRequestResult::Type::Failed)
		return;

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
	bCanAttack = false;
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
	if (bCanAttack == false || bMoveStarted == false || Result != EPathFollowingResult::Type::Success)
	{
		return;
	}
	bMoveStarted = false;
	bAttacking = true;
}