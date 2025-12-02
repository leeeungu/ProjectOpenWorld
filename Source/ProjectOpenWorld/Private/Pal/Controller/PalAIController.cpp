#include "Pal/Controller/PalAIController.h"
#include "Navigation/PathFollowingComponent.h"

bool APalAIController::MoveToActor(AActor* TargetActor, float fAcceptanceRadius )
{
	FAIMoveRequest MoveReq(TargetActor);
	MoveReq.SetUsePathfinding(true);
	MoveReq.SetAllowPartialPath(false);
	MoveReq.SetAcceptanceRadius(fAcceptanceRadius);
	MoveReq.SetReachTestIncludesAgentRadius(true);
	MoveReq.SetCanStrafe(true);
	MoveReq.SetReachTestIncludesGoalRadius(true);
	FNavPathSharedPtr OutPath{};
	if (EPathFollowingRequestResult::Failed == MoveTo(MoveReq, &OutPath))
	{
		UE_LOG(LogTemp, Error, TEXT("MoveToActor :: None Path"));
		return false;
	}
	return true;
}

bool APalAIController::MoveToLocation(FVector TargetLocation, float fAcceptanceRadius )
{
	FAIMoveRequest MoveReq(TargetLocation);
	MoveReq.SetUsePathfinding(true);
	MoveReq.SetAllowPartialPath(false);
	MoveReq.SetAcceptanceRadius(fAcceptanceRadius);
	MoveReq.SetReachTestIncludesAgentRadius(true);
	MoveReq.SetCanStrafe(true);
	MoveReq.SetReachTestIncludesGoalRadius(true);
	FNavPathSharedPtr OutPath{};
	MoveTo(MoveReq, &OutPath);
	if (!OutPath.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("MoveToLocation :: None Path"));
		return false;
	}
	return true;
}
