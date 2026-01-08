#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PalAIController.generated.h"

UCLASS()
class PROJECTOPENWORLD_API APalAIController : public AAIController
{
	GENERATED_BODY()
private:
	EPathFollowingRequestResult::Type Resut{};

protected:
	bool FindLandscapeBelow(FVector StartLocation , FVector EndLocation, FVector& Result);

public:
	APalAIController();

	EPathFollowingRequestResult::Type MoveToActor(AActor* TargetActor, float fAcceptanceRadius = 40.0f);
	EPathFollowingRequestResult::Type MoveToLocation(FVector TargetLocation, float fAcceptanceRadius = 400.0f);

	UFUNCTION(BlueprintPure, Category = "Pal AI Controller")
	bool GetMoveResult() const;
};
