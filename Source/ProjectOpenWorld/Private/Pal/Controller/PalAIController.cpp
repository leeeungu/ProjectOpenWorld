#include "Pal/Controller/PalAIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Blueprint/AIAsyncTaskBlueprintProxy.h"
#include "NavigationSystem.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

APalAIController::APalAIController() : AAIController{}
{
	//GetPathFollowingComponent()->block
	//OnMoveCompleted.AddDynamic(this, &APalAIController::OnMoveCompletedHandler);
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
EPathFollowingRequestResult::Type APalAIController::MoveToActor(AActor* TargetActor, float fAcceptanceRadius)
{
	if (!TargetActor)
		return EPathFollowingRequestResult::Failed;
	// 왜 이동에 오차가 생길까
	//FVector Location = TargetActor->GetActorLocation();
	////Location.Z = GetPawn()->GetActorLocation().Z;

	//GetCharacter()->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_NavWalking, 0) ;

	FAIMoveRequest MoveReq(TargetActor);
	MoveReq.SetUsePathfinding(true);
	MoveReq.SetAllowPartialPath(true);
	MoveReq.SetAcceptanceRadius(fAcceptanceRadius );
	MoveReq.SetReachTestIncludesAgentRadius(false);
	MoveReq.SetCanStrafe(true);
	MoveReq.SetReachTestIncludesGoalRadius(false);
	MoveReq.SetRequireNavigableEndLocation(false);
	FPathFollowingRequestResult result = AAIController::MoveTo(MoveReq);
	Resut = result.Code;
	if (Resut == EPathFollowingRequestResult::Failed)
	{
		UE_LOG(LogTemp, Error, TEXT("MoveToActor :: None Path"));
	}
	if (Resut == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		UE_LOG(LogTemp, Warning, TEXT("MoveToActor :: Already At Goal %f %f %f"), TargetActor->GetDistanceTo(GetPawn()),
			FVector::Dist2D(TargetActor->GetActorLocation(), GetPawn()->GetActorLocation()), fAcceptanceRadius);
	}
	if (Resut == EPathFollowingRequestResult::RequestSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("MoveToActor :: RequestSuccessful %f %f %f %s"), TargetActor->GetDistanceTo(GetPawn()),
			FVector::Dist2D(TargetActor->GetActorLocation(), GetPawn()->GetActorLocation()), fAcceptanceRadius, *result.MoveId.ToString());
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
	MoveReq.SetCanStrafe(true);
	MoveReq.SetReachTestIncludesGoalRadius(true);
	MoveReq.SetRequireNavigableEndLocation(true);
	FNavPathSharedPtr OutPath{};
	FPathFollowingRequestResult result = MoveTo(MoveReq, &OutPath);
	Resut = result.Code;
	if (result.Code == EPathFollowingRequestResult::Failed && !OutPath.IsValid())
	{
		FVector NewLocation{};
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
		else
		{
			UE_LOG(LogTemp, Error, TEXT("APalAIController::MoveToLocation :: None Path"));
		}
	}
	return result.Code;
}
bool APalAIController::GetMoveResult() const
{
	return  Resut != EPathFollowingRequestResult::Failed;
}