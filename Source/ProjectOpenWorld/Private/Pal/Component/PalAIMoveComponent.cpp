#include "Pal/Component/PalAIMoveComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Pal/FunctionLibrary/PalAIBlackboardKeysLibrary.h"

void UPalAIMoveComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerAIController = Cast<AAIController>(GetOwner());
	if (OwnerAIController)
	{
		OwnerAIController->ReceiveMoveCompleted.AddUniqueDynamic(this, &UPalAIMoveComponent::MoveCompleted);
	}
}

void UPalAIMoveComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (TargetActor.IsValid())
	{
		TargetActor->OnDestroyed.RemoveDynamic(this, &UPalAIMoveComponent::TargetDestroyed);
	}
	if (OwnerAIController)
	{
		OwnerAIController->ReceiveMoveCompleted.RemoveDynamic(this, &UPalAIMoveComponent::MoveCompleted);
	}
	Super::EndPlay(EndPlayReason);
}

void UPalAIMoveComponent::TargetDestroyed(AActor* DestroyedActor)
{
	if (TargetActor.IsValid() && TargetActor.Get() == DestroyedActor)
	{
		ResetTargetActor();
	}
}

void UPalAIMoveComponent::MoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	//ResetMove();
	switch (Result)
	{
	case EPathFollowingResult::Success:
		SetStatus(EPalMoveStatus::Arrived);
		break;
	case EPathFollowingResult::Aborted:
		SetStatus(EPalMoveStatus::Idle);
		break;
	default:
		SetStatus(EPalMoveStatus::Failed);
		break;
	}
}

void UPalAIMoveComponent::SetStatus(EPalMoveStatus NewStatus)
{
	if (MoveStatus == NewStatus) 
		return;
	const EPalMoveStatus Old = MoveStatus;
	MoveStatus = NewStatus;
	OnMoveStatusChanged.Broadcast(Old, NewStatus);
}

void UPalAIMoveComponent::StopMove()
{
	if (OwnerAIController)
	{
		OwnerAIController->StopMovement();
	}
	SetStatus(EPalMoveStatus::Idle);
}

void UPalAIMoveComponent::ResetMove()
{
	SetStatus(EPalMoveStatus::Idle);
}

void UPalAIMoveComponent::SetTargetActor(AActor* Target, float fAcceptanceRadius)
{
	AActor* NewTarget = Target;
	if (TargetActor.IsValid())
	{
		TargetActor->OnDestroyed.RemoveDynamic(this, &UPalAIMoveComponent::TargetDestroyed);
	}
	TargetActor = Target;
	if (TargetActor.IsValid())
	{
		TargetActor->OnDestroyed.AddDynamic(this, &UPalAIMoveComponent::TargetDestroyed);
		ActorAcceptanceRadius = fAcceptanceRadius;
	}
	if (OwnerAIController && OwnerAIController->GetBlackboardComponent())
	{
		OwnerAIController->GetBlackboardComponent()->SetValueAsObject(UPalAIBlackboardKeysLibrary::GetBBTargetActorKey(), NewTarget);
	}
}

bool UPalAIMoveComponent::MoveToTargetActor()
{
	TargetActor = GetTargetActor();
	if (MoveStatus == EPalMoveStatus::Moving || !OwnerAIController || !TargetActor.IsValid())
		return false;
	FAIMoveRequest MoveReq(TargetActor.Get());
	MoveReq.SetUsePathfinding(true);
	MoveReq.SetAllowPartialPath(true);
	MoveReq.SetAcceptanceRadius(ActorAcceptanceRadius);
	MoveReq.SetReachTestIncludesAgentRadius(true);
	MoveReq.SetCanStrafe(true);
	MoveReq.SetReachTestIncludesGoalRadius(true);
	MoveReq.SetRequireNavigableEndLocation(true);
	FNavPathSharedPtr OutPath{};
	FPathFollowingRequestResult result = OwnerAIController->MoveTo(MoveReq, &OutPath);
	if (result.Code == EPathFollowingRequestResult::Failed)
	{
		SetStatus(EPalMoveStatus::Failed);
		return false;
	}
	SetStatus(EPalMoveStatus::Moving);
	return true;
}

AActor* UPalAIMoveComponent::GetTargetActor() const
{
	if (OwnerAIController && OwnerAIController->GetBlackboardComponent())
	{
		return Cast< AActor>(OwnerAIController->GetBlackboardComponent()->GetValueAsObject(UPalAIBlackboardKeysLibrary::GetBBTargetActorKey()));
	}
	return nullptr;
}

void UPalAIMoveComponent::ResetTargetActor()
{
	if (TargetActor.IsValid())
	{
		TargetActor->OnDestroyed.RemoveDynamic(this, &UPalAIMoveComponent::TargetDestroyed);
	}
	TargetActor = nullptr;
	if (OwnerAIController && OwnerAIController->GetBlackboardComponent())
	{
		OwnerAIController->GetBlackboardComponent()->ClearValue(UPalAIBlackboardKeysLibrary::GetBBTargetActorKey());
	}
}

void UPalAIMoveComponent::SetTargetLocation(FVector Target, float fAcceptanceRadius)
{
	TargetLocation = Target;
	LocationAcceptanceRadius = fAcceptanceRadius;
	if (OwnerAIController && OwnerAIController->GetBlackboardComponent())
	{
		OwnerAIController->GetBlackboardComponent()->SetValueAsVector(UPalAIBlackboardKeysLibrary::GetBBTargetLocationKey(), TargetLocation);
	}
}

bool UPalAIMoveComponent::MoveToTargetLocation()
{
	if (MoveStatus == EPalMoveStatus::Moving || !OwnerAIController || !GetTargetLocation(TargetLocation))
		return false;

	FAIMoveRequest MoveReq(TargetLocation);
	MoveReq.SetUsePathfinding(true);
	MoveReq.SetAllowPartialPath(true);
	MoveReq.SetAcceptanceRadius(LocationAcceptanceRadius);
	MoveReq.SetReachTestIncludesAgentRadius(true);
	MoveReq.SetCanStrafe(true);
	MoveReq.SetReachTestIncludesGoalRadius(true);
	MoveReq.SetRequireNavigableEndLocation(true);
	FNavPathSharedPtr OutPath{};
	FPathFollowingRequestResult result = OwnerAIController->MoveTo(MoveReq, &OutPath);
	if (result.Code == EPathFollowingRequestResult::Failed)
	{
		SetStatus(EPalMoveStatus::Failed);
		return false;
	}
	SetStatus(EPalMoveStatus::Moving);
	return true;
}

bool UPalAIMoveComponent::GetTargetLocation(FVector& Target) const
{
	if (OwnerAIController && OwnerAIController->GetBlackboardComponent())
	{
		Target = OwnerAIController->GetBlackboardComponent()->GetValueAsVector(UPalAIBlackboardKeysLibrary::GetBBTargetLocationKey());
		return TargetLocation.Equals(Target);
	}
	Target = FVector::ZeroVector;
	return false;
}

void UPalAIMoveComponent::ResetTargetLocation()
{
	TargetLocation = FVector::ZeroVector;
	if (OwnerAIController && OwnerAIController->GetBlackboardComponent())
	{
		OwnerAIController->GetBlackboardComponent()->SetValueAsVector(UPalAIBlackboardKeysLibrary::GetBBTargetLocationKey(), TargetLocation);
	}
}
