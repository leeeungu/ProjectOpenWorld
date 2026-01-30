#include "Item/AssetUserData/ItemSpawnListAssetUserData.h"

FName UItemSpawnListAssetUserData::GetRandomItemSpawn() const
{
	// Calculate the total spawn rate
	float TotalSpawnRate = 0.0f;
	for (float Rate : ItemSpawnRateList)
	{
		TotalSpawnRate += Rate;
	}
	if (TotalSpawnRate > 0.0f && ItemSpawnList.Num() == ItemSpawnRateList.Num())
	{
		// Generate a random value between 0 and TotalSpawnRate
		float RandomValue = FMath::FRandRange(0.0f, TotalSpawnRate);
		// Determine which item to spawn based on the random value
		float CumulativeRate = 0.0f;
		for (int32 i = 0; i < ItemSpawnRateList.Num(); ++i)
		{
			CumulativeRate += ItemSpawnRateList[i];
			if (RandomValue <= CumulativeRate)
			{
				return ItemSpawnList[i];
			}
		}
	}

	return FName();
}
