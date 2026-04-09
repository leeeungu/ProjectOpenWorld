#pragma once

#include "CoreMinimal.h"
#include "Item/Object/ItemDataFragment.h"
#include "Engine/DataTable.h"
#include "DataTableItemFragment.generated.h"

namespace ITEMDATATABLENAMING
{
	const FName PalStaticItemDataTable = TEXT("PalStaticItemDataTable");
	const FName PalItemRecipeDataTable = TEXT("PalItemRecipeDataTable");
	const FName PalItemIconDataTable = TEXT("PalItemIconDataTable");
};

class UDataTable;

UCLASS()
class PROJECTOPENWORLD_API UDataTableItemFragment : public UItemDataFragment
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "DataTable", meta = (GetKeyOptions = GetAllDataTableNames))
	TMap<FName, TSoftObjectPtr<UDataTable>> PalStaticItemDataTableMap{};
public:
	template<typename T>
	bool GetData(FName Key, TArray<T*>& OutPut) const;
private:
	UFUNCTION()
	TArray<FName> GetAllDataTableNames()
	{
		TArray<FName> DataTableNames{};
		DataTableNames.Add(ITEMDATATABLENAMING::PalStaticItemDataTable);
		DataTableNames.Add(ITEMDATATABLENAMING::PalItemRecipeDataTable);
		DataTableNames.Add(ITEMDATATABLENAMING::PalItemIconDataTable);
		return DataTableNames;
	}
};


template<typename T>
 bool UDataTableItemFragment::GetData(FName Key,  TArray<T*>& OutPut) const
{
	if (const TSoftObjectPtr<UDataTable>* FoundDataTable = PalStaticItemDataTableMap.Find(Key))
	{
		if (FoundDataTable->IsValid())
		{
			UDataTable* DataTable = FoundDataTable->LoadSynchronous();
			DataTable->GetAllRows<T>("", OutPut);
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Data table '%s' is not valid."), *Key.ToString());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Data table with key '%s' not found in the map."), *Key.ToString());
	}

	return false;
}
