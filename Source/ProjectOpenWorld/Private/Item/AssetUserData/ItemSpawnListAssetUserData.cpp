#include "Item/AssetUserData/ItemSpawnListAssetUserData.h"

FItemSpawnRateData UItemSpawnListAssetUserData::GetRandomItem() const
{
	FItemSpawnRateData Data{};
	int MaxIndex = ItemSpawnList.Num();
	int RandomValue = FMath::RandRange(1, 100);
	int i = 0;
	for (i; i < MaxIndex - 1; i++)
	{
		const FItemSpawnRateData& Rate = ItemSpawnList[i];
		const FItemSpawnRateData* NextRate = &ItemSpawnList[i + 1];
		if (RandomValue >= Rate.SpawnRangePrefixSum && RandomValue< NextRate->SpawnRangePrefixSum)
		{
			break;
		}
	}
	Data = ItemSpawnList[i];
	Data.SpawnRandomCount = FMath::RandRange(1, Data.SpawnRandomCount);
	return Data;
}

void UItemSpawnListAssetUserData::PostEditChangeOwner()
{
	Super::PostEditChangeOwner();
	if (ItemSpawnDTListName != NAME_None)
	{
		UDataTable* DataTable = nullptr;
		//Script/Engine.DataTable'/Game/Item/DataTable/DT_ItemSpawnListUserData.DT_ItemSpawnListUserData'
		DataTable = LoadObject<UDataTable>(nullptr, *StaticGetDataTablePath());
		if (DataTable)
		{
			FItemSpawnListDataTableRow* Data = DataTable->FindRow<FItemSpawnListDataTableRow>(ItemSpawnDTListName, TEXT("UItemSpawnListAssetUserData::PostEditChangeOwner"));
			if (Data)
			{
				ItemSpawnList = Data->ItemSpawnList;
			}
			else
			{
				FItemSpawnListDataTableRow NewData{};
				NewData.ItemSpawnList = ItemSpawnList;
				DataTable->AddRow(ItemSpawnDTListName, NewData);
				UE_LOG(LogTemp, Warning, TEXT("UItemSpawnListAssetUserData::PostEditChangeOwner : DataTable Row Added"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UItemSpawnListAssetUserData::PostEditChangeOwner : DataTable Not Found"));
		}
	}
	if (bEditItemSpawnListAsset)
	{
		float TotalSpawnRate = 0.0f;
		for (auto& Rate : ItemSpawnList)
		{
			TotalSpawnRate += Rate.SpawnRandomRate;
		}
		int Index{};
		FItemSpawnRateData* PreviousRate = nullptr;
		int MaxIndex = ItemSpawnList.Num() -1;
		for (int i = MaxIndex; i >= 0; i--)
		{
			FItemSpawnRateData& Rate = ItemSpawnList[i];
			Rate.SpawnPercent = Rate.SpawnRandomRate / TotalSpawnRate;
			Rate.SpawnRangePrefixSum = 100  - Rate.SpawnPercent * 100;
			if (PreviousRate)
			{
				Rate.SpawnRangePrefixSum -= (100 - PreviousRate->SpawnRangePrefixSum );
			}
			if (Rate.SpawnRangePrefixSum < 0)
				Rate.SpawnRangePrefixSum = 0;
			PreviousRate = &Rate;
		}
	}
	//FItemSpawnRateData Data = GetRandomItem();
	//UE_LOG(LogTemp, Warning, TEXT("Random Item : %s"), *Data.ItemName.ToString());
}
