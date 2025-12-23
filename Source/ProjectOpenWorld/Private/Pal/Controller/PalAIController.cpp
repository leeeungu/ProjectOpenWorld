#include "Pal/Controller/PalAIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Blueprint/AIAsyncTaskBlueprintProxy.h"
#include "NavigationSystem.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Landscape.h"

APalAIController::APalAIController() : AAIController{}
{
	//GetPathFollowingComponent()->block

}

bool APalAIController::FindLandscapeBelow(FVector StartLocation, FVector EndLocation, FVector& Result)
{
	// Define the collision channel for the raycast
	const ECollisionChannel TraceChannel = ECC_Visibility;

	// Perform the line trace
	TArray<FHitResult> OutHits{};
	bool bHit = GetWorld()->LineTraceMultiByChannel(
		OutHits,
		StartLocation ,
		EndLocation,
		TraceChannel
	);


	// Check if the hit actor is a Landscape
	Result = StartLocation;
	//if (bHit)
	{
		for (FHitResult& OutHit : OutHits)
		{
			if (OutHit.GetActor())
			{
				Result = OutHit.ImpactPoint;
				UE_LOG(LogTemp, Warning, TEXT("FindLandscapeBelow :: Found at Location: %s"), *Result.ToString());
				DrawDebugLine(GetWorld(), StartLocation, Result, FColor::Green, false, 5.0f, 0, 2.0f);
				return true;
			}
		}
	}
	return false;
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
		/*FVector NewLocation{};
		if (FindLandscapeBelow(TargetLocation, TargetLocation + FVector(0, 0, -600), NewLocation))
		{
			MoveReq.SetGoalLocation(NewLocation);
			result = MoveTo(MoveReq, &OutPath);
			if (result.Code == EPathFollowingRequestResult::Failed && !OutPath.IsValid())
			{
					GetPawn()->SetActorLocation(NewLocation);
				UE_LOG(LogTemp, Error, TEXT("APalAIController::MoveToLocation :: None Path Even After Landscape Adjust"));
				return EPathFollowingRequestResult::AlreadyAtGoal;
			}
		}
		else*/
		{
			UE_LOG(LogTemp, Error, TEXT("APalAIController::MoveToLocation :: None Path"));
		}
	}


	return result.Code;
}
