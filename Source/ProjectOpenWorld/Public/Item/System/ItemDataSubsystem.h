#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Item/DataTable/PalStaticItemDataStruct.h"
#include "ItemDataSubsystem.generated.h"

class AItemActor;

UCLASS()
class PROJECTOPENWORLD_API UItemDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
protected:
	TObjectPtr<UDataTable> PalStaticItemDataTable{};

	TMap<int64, FPalStaticItemDataStruct*> PalStaticItemDataMap{}; // Key: Editor_RowNameHash

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	FPalStaticItemDataStruct GetPalStaticItemDataByHash(int64 RowNameHash) const;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	FString GetPalStaticItemOverrideNameByHash(int64 RowNameHash) const;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	FString GetPalStaticItemOverrideDescriptionByHash(int64 RowNameHash) const;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	FString GetPalStaticItemIconNameByHash(int64 RowNameHash) const;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	int32 GetPalStaticItemMaxStackCountByHash(int64 RowNameHash) const;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	float GetPalStaticItemWeightByHash(int64 RowNameHash) const;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	int32 GetPalStaticItemPriceByHash(int64 RowNameHash) const;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	int32 GetPalStaticItemSortIDByHash(int64 RowNameHash) const;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	TSoftClassPtr<AItemActor> GetPalStaticItemVisualBlueprintClassSoftByHash(int64 RowNameHash) const;
};
