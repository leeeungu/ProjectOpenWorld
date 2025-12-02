#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PalAIController.generated.h"

UCLASS()
class PROJECTOPENWORLD_API APalAIController : public AAIController
{
	GENERATED_BODY()
public:

	bool MoveToActor(AActor* TargetActor, float fAcceptanceRadius = 400.0f);
	bool MoveToLocation(FVector TargetLocation, float fAcceptanceRadius = 400.0f);
};
