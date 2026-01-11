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
	static UItemDataSubsystem* SingletonInstance;
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

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	static bool IsValidInstance() { return SingletonInstance != nullptr; }
	static bool GetPalStaticItemDataPtr(FName RowName, const FPalStaticItemDataStruct*& Data) ;
	static bool GetPalItemRecipeDataPtr(FName RowName, const FPalItemRecipe*& Data) ;
	static bool GetPalItemIconDataPtr(FName RowName, const FPalEditorItemIconTableRow*& Data) ;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	static FPalStaticItemDataStruct GetPalStaticItemDataByName(FName RowName) ;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	static FString GetPalStaticItemOverrideNameByName(FName RowName) ;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	static FString GetPalStaticItemOverrideDescriptionByName(FName RowName) ;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	static FString GetPalStaticItemIconNameByName(FName RowName) ;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	static int32 GetPalStaticItemMaxStackCountByName(FName RowName) ;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	static float GetPalStaticItemWeightByName(FName RowName) ;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	static int32 GetPalStaticItemPriceByName(FName RowName) ;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	static int32 GetPalStaticItemSortIDByName(FName RowName) ;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	static TSoftClassPtr<AItemActor> GetPalStaticItemVisualBlueprintClassSoftByName(FName RowName) ;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	static FString GetPalItemRecipeProductIdByName(FName RowName) ;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	static int32 GetPalItemRecipeProductCountByName(FName RowName) ;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	static float GetPalItemRecipeWorkAmountByName(FName RowName) ;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	static FString GetPalItemRecipeUnlockItemIDByName(FName RowName) ;

	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	static const TArray< FRecipeMaterialData>& GetPalItemRecipeMaterialsByName(FName RowName) ;


	UFUNCTION(BlueprintPure, Category = "ItemDataSubsystem")
	static UTexture2D* GetPalItemIconTextureByName(FName RowName) ;
};
