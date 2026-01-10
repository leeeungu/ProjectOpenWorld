#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "Item/DataTable/PalStaticItemDataStruct.h"
#include "Item/DataTable/PalItemRecipe.h"
#include "Item/DataTable/PalEditorItemIconTableRow.h"
#include "UObject/UObjectGlobals.h"
#include "ItemDataSubsystem.generated.h"

class AItemActor;

UCLASS()
class PROJECTOPENWORLD_API UItemDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
private:
	template<typename T>
	struct DataTableStruct
	{
		TObjectPtr<UDataTable> ItemDataTable{};
		TMap<FName, const T*> ItemDataMap{}; // Key: Editor_RowNameHash
		T Dummy{};
	};

protected:

	DataTableStruct< FPalStaticItemDataStruct> PalStaticItemDataTableStruct{};
	DataTableStruct< FPalItemRecipe> PalItemRecipeDataTableStruct{};
	DataTableStruct< FPalEditorItemIconTableRow> PalItemIconDataTableStruct{};

	template<typename T>
	bool LoadAndSaveDataTableToMap(DataTableStruct<T>& DataStruct, FString Path)
	{
		TObjectPtr<UDataTable>& DataTable = DataStruct.ItemDataTable;
		DataTable = LoadObject<UDataTable>(nullptr, *Path);
		if (!DataTable)
		{
			return false;	
		}
		TArray<FName> RowNames = DataTable->GetRowNames();
		for (const FName& RowName : RowNames)
		{
			T* Data = DataTable->FindRow<T>(RowName, "");
			if (Data)
			{
				DataStruct.ItemDataMap.Add(RowName, Data);
			}
		}
		return true;
	}

	bool GetPalStaticItemDataPtr(FName RowName, const FPalStaticItemDataStruct*& Data) const;
	bool GetPalItemRecipeDataPtr(FName RowName, const FPalItemRecipe*& Data) const;
	bool GetPalItemIconDataPtr(FName RowName, const FPalEditorItemIconTableRow*& Data) const;
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	FPalStaticItemDataStruct GetPalStaticItemDataByName(FName RowName) const;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	FString GetPalStaticItemOverrideNameByName(FName RowName) const;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	FString GetPalStaticItemOverrideDescriptionByName(FName RowName) const;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	FString GetPalStaticItemIconNameByName(FName RowName) const;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	int32 GetPalStaticItemMaxStackCountByName(FName RowName) const;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	float GetPalStaticItemWeightByName(FName RowName) const;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	int32 GetPalStaticItemPriceByName(FName RowName) const;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	int32 GetPalStaticItemSortIDByName(FName RowName) const;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	TSoftClassPtr<AItemActor> GetPalStaticItemVisualBlueprintClassSoftByName(FName RowName) const;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	FString GetPalItemRecipeProductIdByName(FName RowName) const;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	int32 GetPalItemRecipeProductCountByName(FName RowName) const;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	float GetPalItemRecipeWorkAmountByName(FName RowName) const;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	FString GetPalItemRecipeUnlockItemIDByName(FName RowName) const;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	const TArray< FMaterialData>& GetPalItemRecipeMaterialsByName(FName RowName) const;


	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	UTexture2D* GetPalItemIconTextureByName(FName RowName) const;
};
