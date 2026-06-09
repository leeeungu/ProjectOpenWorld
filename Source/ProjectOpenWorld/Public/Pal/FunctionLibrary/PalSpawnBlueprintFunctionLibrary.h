#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PalSpawnBlueprintFunctionLibrary.generated.h"

class AItemActor;
class AActor;

UCLASS()
class PROJECTOPENWORLD_API UPalSpawnBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
   // UFUNCTION(BlueprintCallable, Category = "Pal |Item")
   // static AItemActor* SpawnItemsFromMesh(UWorld* World, FName ItemName, int32 Count, FTransform Origin, AActor* Owner = nullptr);
    UFUNCTION(BlueprintCallable, Category = "Pal|Item")
    static int32 SpawnItemsForCharacter(UObject* WorldContext, FName CharacterID, const FTransform& Origin, AActor* Causer = nullptr);

    UFUNCTION(BlueprintCallable, Category = "Pal|Item")
    static AItemActor* SpawnPalStaticItemVisualActorByName(UObject* WorldContext, FName ItemName, const FTransform& Origin, int Count = 1);

};
