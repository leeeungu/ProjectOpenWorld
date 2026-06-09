#include "Pal/FunctionLibrary/PalSpawnBlueprintFunctionLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Pal/Subsystem/PalCharacterDataSubsystem.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Item/Actor/ItemActor.h"
#include "Pal/FunctionLibrary/PalSpawnBlueprintFunctionLibrary.h"

AItemActor* UPalSpawnBlueprintFunctionLibrary::SpawnPalStaticItemVisualActorByName(UObject* WorldContext, FName ItemName, const FTransform& Origin, int Count)
{
	if (WorldContext == nullptr)
		return nullptr;
	UWorld* World = WorldContext->GetWorld();
	if (!World)
		return nullptr;
	const FPalStaticItemDataStruct* Result{};
	if (!UItemDataSubsystem::GetPalStaticItemDataPtr(ItemName, &Result))
	{
		UE_LOG(LogTemp, Warning, TEXT("UPalSpawnBlueprintFunctionLibrary::SpawnPalStaticItemVisualActorByName: No data found for ItemID '%s'."), *ItemName.ToString());
	}
	TSubclassOf<UObject> SpawnClass = Result->VisualBlueprintClassSoft;
	if (World && Result->VisualBlueprintClassSoft == nullptr)
	{
		// Script / Engine.Blueprint'/Game/Item/Blueprint/Base/BP_BaseItem.BP_BaseItem'
		SpawnClass = LoadClass<UObject>(World, TEXT("/Game/Item/Blueprint/Base/BP_BaseItem.BP_BaseItem_C"));
	}
	if (World && SpawnClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		AItemActor* SpawnedActor = World->SpawnActor<AItemActor>(SpawnClass, Origin, SpawnParams);
		if (SpawnedActor)
		{
			SpawnedActor->Init(ItemName, Count);
		}
		return SpawnedActor;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UPalSpawnBlueprintFunctionLibrary::Failed to spawn item actor. World or SpawnClass is null."));
	}
	return nullptr;
}

int32 UPalSpawnBlueprintFunctionLibrary::SpawnItemsForCharacter(UObject* WorldContext, FName CharacterID, const FTransform& Origin, AActor* Causer)
{
	UWorld* World = WorldContext->GetWorld();
	if (!World)
		return -1;
	TArray<FPalItemDropData> DropItemList = UPalCharacterDataSubsystem::GetDropItemListByCharacterID(CharacterID);
	int32 Counts{};
	for (const FPalItemDropData& ItemData : DropItemList)
	{
		int Rate = FMath::RandRange(1, 100);
		if (Rate < ItemData.Rate)
		{
			int nCount = FMath::RandRange(ItemData.min, ItemData.Max);
			UPalSpawnBlueprintFunctionLibrary::SpawnPalStaticItemVisualActorByName(World, ItemData.ItemId, Origin, nCount);
			Counts++;
		}
	}
	return Counts;
}
