#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PalAIBlackboardKeysLibrary.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UPalAIBlackboardKeysLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
    UFUNCTION(BlueprintPure, Category = "PalAI|BBKeys")
    static FName GetBBTargetActorKey();

    UFUNCTION(BlueprintPure, Category = "PalAI|BBKeys")
    static FName GetBBTargetLocationKey();

    UFUNCTION(BlueprintPure, Category = "PalAI|BBKeys")
    static FName GetBBMonsterStateKey();

    UFUNCTION(BlueprintPure, Category = "PalAI|BBKeys")
    static FName GetBBJobType();
    UFUNCTION(BlueprintPure, Category = "PalAI|BBKeys")
    static FName GetBBJobTarget();
    UFUNCTION(BlueprintPure, Category = "PalAI|BBKeys")
    static FName GetBBJobLocation();

    UFUNCTION(BlueprintPure, Category = "PalAI|BBKeys")
    static FName GetBBOwnerPlayer();
    
};
