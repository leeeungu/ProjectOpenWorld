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
public:
	UFUNCTION(BlueprintPure, Category = "PalCommand")
	static FPalCommand CommandMoveToLocation(AActor* pInstigator, FVector TargetLocation, float MoveDistance = 50.0f);
	UFUNCTION(BlueprintPure, Category = "PalCommand")
	static FPalCommand CommandMoveToActor(AActor* pInstigator, AActor* pTargetActor, float MoveDistance = 50.0f);
	UFUNCTION(BlueprintPure, Category = "PalCommand")
	static FPalCommand CommandArchitecture(AActor* pInstigator, AActor* pTargetActor);
	// pDestination final , pTargetActor 옮길 물체
	UFUNCTION(BlueprintPure, Category = "PalCommand")
	static FPalCommand CommandTransport(AActor* pInstigator, AActor* pTargetActor, AActor* pDestination);

	UFUNCTION(BlueprintPure, Category = "PalCommand")
	static FPalCommand CommandMining(AActor* pInstigator, AActor* pTargetActor);

	UFUNCTION(BlueprintPure, Category = "PalCommand")
	static FPalCommand CommandAttack(AActor* pInstigator, AActor* pTargetActor, ESubAttackType SlotNum);
};
