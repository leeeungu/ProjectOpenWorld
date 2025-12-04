#include "Pal/Controller/PalAIController.h"
#include "Navigation/PathFollowingComponent.h"

EPathFollowingRequestResult::Type APalAIController::MoveToActor(AActor* TargetActor, float fAcceptanceRadius )
{
	if (!TargetActor)
		return EPathFollowingRequestResult::Failed;
	FAIMoveRequest MoveReq(TargetActor);
	MoveReq.SetUsePathfinding(true);
	MoveReq.SetAllowPartialPath(true);
	MoveReq.SetAcceptanceRadius(fAcceptanceRadius);
	MoveReq.SetReachTestIncludesAgentRadius(true);
	MoveReq.SetCanStrafe(true);
	MoveReq.SetReachTestIncludesGoalRadius(true);
	FNavPathSharedPtr OutPath{};
	FPathFollowingRequestResult result = MoveTo(MoveReq, &OutPath);
	if (result.Code == EPathFollowingRequestResult::Failed && !OutPath.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("MoveToActor :: None Path"));
	}
	return result;
}

EPathFollowingRequestResult::Type APalAIController::MoveToLocation(FVector TargetLocation, float fAcceptanceRadius )
{
	FAIMoveRequest MoveReq(TargetLocation);
	MoveReq.SetUsePathfinding(true);
	MoveReq.SetAllowPartialPath(true);
	MoveReq.SetAcceptanceRadius(fAcceptanceRadius);
	MoveReq.SetReachTestIncludesAgentRadius(true);
	MoveReq.SetCanStrafe(false);
	MoveReq.SetReachTestIncludesGoalRadius(true);
	MoveReq.SetRequireNavigableEndLocation(true);
	FNavPathSharedPtr OutPath{};
	FPathFollowingRequestResult result = MoveTo(MoveReq, &OutPath);
	if (result.Code == EPathFollowingRequestResult::Failed && !OutPath.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("MoveToLocation :: None Path"));
	}
	return result;
}
