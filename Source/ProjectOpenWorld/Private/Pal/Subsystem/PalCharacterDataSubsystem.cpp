#include "Pal/Subsystem/PalCharacterDataSubsystem.h"

UPalCharacterDataSubsystem* UPalCharacterDataSubsystem::SingletonInstance{};

void UPalCharacterDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	SingletonInstance = this;

	//Script/Engine.DataTable'/Game/Pal/DataTable/DT_PalCharacterIconDataTable.DT_PalCharacterIconDataTable'
	if (!LoadAndSaveDataTableToMap(PalCharacterIconDataTable, TEXT("/Game/Pal/DataTable/DT_PalCharacterIconDataTable.DT_PalCharacterIconDataTable")))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load PalCharacterIconData DataTable"));
	}
	//Script/Engine.DataTable'/Game/Item/DataTable/DT_PalDropItem.DT_PalDropItem'
	if (!LoadAndSaveDataTableToMap(PalDropItemDatabaseTable, TEXT("/Game/Item/DataTable/DT_PalDropItem.DT_PalDropItem")))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load PalDropItemDatabase DataTable"));
	}

	//Script/Engine.DataTable'/Game/Pal/DataTable/DT_SpawnCharacterData.DT_SpawnCharacterData'
	if (!LoadAndSaveDataTableToMap(PalSpawnCharacterDatabaseTable, TEXT("/Game/Pal/DataTable/DT_SpawnCharacterData.DT_SpawnCharacterData")))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load DT_SpawnCharacterData DataTable"));
	}
}

bool UPalCharacterDataSubsystem::GetPalCharacterIconData(FName RowName, const FPalCharacterIconDataRow*& Data)
{
	if (!SingletonInstance)
		return false;
	if (const FPalCharacterIconDataRow* const* FoundData = SingletonInstance->PalCharacterIconDataTable.DataMap.Find(RowName))
	{
		Data = *FoundData;
		return true;
	}
	Data = &SingletonInstance->PalCharacterIconDataTable.Dummy;
	return false;
}

bool UPalCharacterDataSubsystem::GetPalDropItemData(FName RowName, const FPalDropItemDatabaseRow*& Data)
{
	if (!SingletonInstance)
		return false;
	if (const FPalDropItemDatabaseRow* const* FoundData = SingletonInstance->PalDropItemDatabaseTable.DataMap.Find(RowName))
	{
		Data = *FoundData;
		return true;
	}
	Data = &SingletonInstance->PalDropItemDatabaseTable.Dummy;
	return false;
}

bool UPalCharacterDataSubsystem::GetPalSpawnCharacterData(FName RowName, const FSpawnCharacterData*& Data)
{
	if (!SingletonInstance)
		return false;
	if (const FSpawnCharacterData* const* FoundData = SingletonInstance->PalSpawnCharacterDatabaseTable.DataMap.Find(RowName))
	{
		Data = *FoundData;
		return true;
	}
	Data = &SingletonInstance->PalSpawnCharacterDatabaseTable.Dummy;
	return false;
}
UTexture2D* UPalCharacterDataSubsystem::GetPalCharacterIconByName(FName CharacterID)
{
	const FPalCharacterIconDataRow* Data = nullptr;
	if (GetPalCharacterIconData(CharacterID, Data))
	{
		return Data->Icon;
	}
	return nullptr;
}

TArray<FPalItemDropData> UPalCharacterDataSubsystem::GetDropItemListByCharacterID(FName CharacterID)
{
	const FPalDropItemDatabaseRow* Data = nullptr;
	if (GetPalDropItemData(CharacterID, Data))
	{
		return Data->DropItemList;
	}
	return TArray<FPalItemDropData>();
}

FSpawnCharacterData UPalCharacterDataSubsystem::GetSpawnCharacterDataByPalName(FName PalName)
{
	const FSpawnCharacterData* Data = nullptr;
	if (GetPalSpawnCharacterData(PalName, Data))
	{
		return *Data;
	}
	return FSpawnCharacterData();
}

