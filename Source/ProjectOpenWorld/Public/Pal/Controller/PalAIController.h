#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PalAIController.generated.h"

UCLASS()
class PROJECTOPENWORLD_API APalAIController : public AAIController
{
	GENERATED_BODY()
public:

	EPathFollowingRequestResult::Type MoveToActor(AActor* TargetActor, float fAcceptanceRadius = 40.0f);
	EPathFollowingRequestResult::Type MoveToLocation(FVector TargetLocation, float fAcceptanceRadius = 400.0f);
};
