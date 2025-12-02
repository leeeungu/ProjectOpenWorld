#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Pal/Data/PalCommandData.h"
#include "PalCommandFunctionLibrary.generated.h"

//enum class ESubMoveType : uint8
//enum class ESubAttackType : uint8
//enum class ESubWorkType : uint8

UCLASS()
class PROJECTOPENWORLD_API UPalCommandFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	UFUNCTION(BlueprintPure, Category = "PalCommand")
	static FPalCommand CommandMoveToLocation(AActor* pInstigator, FVector TargetLocation);
	UFUNCTION(BlueprintPure, Category = "PalCommand")
	static FPalCommand CommandMoveToActor(AActor* pInstigator, AActor* pTargetActor );
	UFUNCTION(BlueprintPure, Category = "PalCommand")
	static FPalCommand CommandArchitecture(AActor* pInstigator, AActor* pTargetActor);
};
