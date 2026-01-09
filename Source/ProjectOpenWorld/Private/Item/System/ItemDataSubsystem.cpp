#include "Item/System/ItemDataSubsystem.h"

void UItemDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	// Initialization logic for item data subsystem goes here

	//Script/Engine.DataTable'/Game/Item/DataTable/DT_ItemDataTable.DT_ItemDataTable'
	PalStaticItemDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Item/DataTable/DT_PalStaticItemData.DT_PalStaticItemData"));
	if (PalStaticItemDataTable)
	{
		const TMap<FName, uint8*>& RowMap = PalStaticItemDataTable->GetRowMap();
		for (const TPair<FName, uint8*>& RowPair : RowMap)
		{
			FPalStaticItemDataStruct* RowData = reinterpret_cast<FPalStaticItemDataStruct*>(RowPair.Value);
			if (RowData)
			{
				// Assuming Editor_RowNameHash is a property of FPalStaticItemDataStruct
				int64 RowNameHash = RowData->Editor_RowNameHash;
				PalStaticItemDataMap.Add(RowNameHash, RowData);
			}
		}
	}
}

FPalStaticItemDataStruct UItemDataSubsystem::GetPalStaticItemDataByHash(int64 RowNameHash) const
{
	if (const FPalStaticItemDataStruct* const* FoundData = PalStaticItemDataMap.Find(RowNameHash))
	{
		return **FoundData;
	}
	return FPalStaticItemDataStruct();
}

FString UItemDataSubsystem::GetPalStaticItemOverrideNameByHash(int64 RowNameHash) const
{
	return GetPalStaticItemDataByHash(RowNameHash).OverrideName;
}

FString UItemDataSubsystem::GetPalStaticItemOverrideDescriptionByHash(int64 RowNameHash) const
{
	return GetPalStaticItemDataByHash(RowNameHash).OverrideDescription;
}

FString UItemDataSubsystem::GetPalStaticItemIconNameByHash(int64 RowNameHash) const
{
	return GetPalStaticItemDataByHash(RowNameHash).IconName;
}

int32 UItemDataSubsystem::GetPalStaticItemMaxStackCountByHash(int64 RowNameHash) const
{
	return GetPalStaticItemDataByHash(RowNameHash).MaxStackCount;
}

float UItemDataSubsystem::GetPalStaticItemWeightByHash(int64 RowNameHash) const
{
	return GetPalStaticItemDataByHash(RowNameHash).Weight;
}

int32 UItemDataSubsystem::GetPalStaticItemPriceByHash(int64 RowNameHash) const
{
	return GetPalStaticItemDataByHash(RowNameHash).Price;
}

int32 UItemDataSubsystem::GetPalStaticItemSortIDByHash(int64 RowNameHash) const
{
	return GetPalStaticItemDataByHash(RowNameHash).SortID;
}

TSoftClassPtr<AItemActor> UItemDataSubsystem::GetPalStaticItemVisualBlueprintClassSoftByHash(int64 RowNameHash) const
{
	return GetPalStaticItemDataByHash(RowNameHash).VisualBlueprintClassSoft;
}
