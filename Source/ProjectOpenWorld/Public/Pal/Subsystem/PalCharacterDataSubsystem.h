#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Item/DataTable/PalDropItemDatabaseRow.h"
#include "Pal/DataTable/PalCharacterIconDataRow.h"
#include "Pal/DataTable/PalWildSpawnerDatabaseRow.h"
#include "PalCharacterDataSubsystem.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UPalCharacterDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
private:
	template<typename T>
	struct DataTableStruct
	{
		UPROPERTY()
		TObjectPtr<UDataTable> DataTableAsset{};
		UPROPERTY()
		TMap<FName, const T*> DataMap{}; // Key: Editor_RowNameHash
		T Dummy{};
	};
	static UPalCharacterDataSubsystem* SingletonInstance;
protected:
	DataTableStruct<FPalCharacterIconDataRow> PalCharacterIconDataTable{};
	DataTableStruct<FPalDropItemDatabaseRow> PalDropItemDatabaseTable{};
	DataTableStruct<FSpawnCharacterData> PalSpawnCharacterDatabaseTable{};

	template<typename T>
	bool LoadAndSaveDataTableToMap(DataTableStruct<T>& DataStruct, FString Path)
	{
		TObjectPtr<UDataTable>& DataTable = DataStruct.DataTableAsset;
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
				DataStruct.DataMap.Add(RowName, Data);
			}
		}
		return true;
	}


public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	static bool GetPalCharacterIconData(FName RowName, const FPalCharacterIconDataRow*& Data);
	static bool GetPalDropItemData(FName RowName, const FPalDropItemDatabaseRow*& Data);
	static bool GetPalSpawnCharacterData(FName RowName, const FSpawnCharacterData*& Data);

	static UTexture2D* GetPalCharacterIconByName(FName CharacterID);
	static TArray< FPalItemDropData> GetDropItemListByCharacterID(FName CharacterID);
	static FSpawnCharacterData GetSpawnCharacterDataByPalName(FName PalName);
};
////LoadAndSaveDataTableToMap specialization for FPalDropItemDatabaseRow
template<>
inline bool UPalCharacterDataSubsystem::LoadAndSaveDataTableToMap<FPalDropItemDatabaseRow>(DataTableStruct<FPalDropItemDatabaseRow >& DataStruct, FString Path)
{
	TObjectPtr<UDataTable>& DataTable = DataStruct.DataTableAsset;
	DataTable = LoadObject<UDataTable>(nullptr, *Path);
	if (!DataTable)
	{
		return false;
	}
	TArray<FName> RowNames = DataTable->GetRowNames();
	for (const FName& RowName : RowNames)
	{
		FPalDropItemDatabaseRow* Data = DataTable->FindRow<FPalDropItemDatabaseRow>(RowName, "");
		if (Data)
		{
			DataStruct.DataMap.Add(*Data->CharacterID.ToString(), Data);
		}
	}
	return true;
}
