#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PalSphereBFL.generated.h"

class APalSphere;

UCLASS()
class PROJECTOPENWORLD_API UPalSphereBFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static FVector GetCurveLocation(UWorld* WorldContext, FVector PreLocation, UCurveFloat* pHeightCurve, float Time, FVector Velocity);
};
