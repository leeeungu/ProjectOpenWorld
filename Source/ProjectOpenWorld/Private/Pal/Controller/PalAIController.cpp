#include "Pal/Controller/PalAIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Blueprint/AIAsyncTaskBlueprintProxy.h"
#include "NavigationSystem.h"

APalAIController::APalAIController() : AAIController{}
{
	//GetPathFollowingComponent()->block

}

EPathFollowingRequestResult::Type APalAIController::MoveToActor(AActor* TargetActor, float fAcceptanceRadius )
{
	if (!TargetActor)
		return EPathFollowingRequestResult::Failed;
	FAIMoveRequest MoveReq(TargetActor);
	MoveReq.SetUsePathfinding(true);
	MoveReq.SetAllowPartialPath(true);
	MoveReq.SetRequireNavigableEndLocation(true);
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
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	//if (NavSystem == nullptr)
	//	return EPathFollowingRequestResult::Type::Failed;
	//UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, TargetLocation);
	////UNavigationPath* Path = UNavigationSystemV1::FindPathToLocationSynchronously(GetWorld(), GetPawn()->GetActorLocation(), TargetLocation);
	////FPathFindingQuery query{};
	////UNavigationSystemV1::FindPathSync(query, EPathFindingMode::Type::Hierarchical);
	////
	////	// AI 한테 찍은 좌표로 이동시키라고 명령
	////auto a = UAIBlueprintHelperLibrary::CreateMoveToProxyObject(GetWorld(), GetPawn(), TargetLocation, nullptr, fAcceptanceRadius, true);
	////a->
	////(this, TargetLocation);
	//UE_LOG(LogTemp, Warning, TEXT("APalAIController::MoveToLocation Start PathFind"));
	//	return EPathFollowingRequestResult::Type::RequestSuccessful;
	FAIMoveRequest MoveReq(TargetLocation);
	MoveReq.SetUsePathfinding(true);
	MoveReq.SetAllowPartialPath(true);
	MoveReq.SetAcceptanceRadius(fAcceptanceRadius);
	MoveReq.SetReachTestIncludesAgentRadius(true);
	MoveReq.SetCanStrafe(true);
	MoveReq.SetReachTestIncludesGoalRadius(true);
	MoveReq.SetRequireNavigableEndLocation(true);
	FNavPathSharedPtr OutPath{};
	FPathFollowingRequestResult result = MoveTo(MoveReq, &OutPath);
	if (result.Code == EPathFollowingRequestResult::Failed && !OutPath.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("APalAIController::MoveToLocation :: None Path"));
	}
	return result.Code;
}
