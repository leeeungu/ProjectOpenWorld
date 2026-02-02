#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "ItemSpawnListAssetUserData.generated.h"

USTRUCT(BlueprintType)
struct FItemSpawnRateData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemSpawnRateData")
	FName ItemName{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemSpawnRateData", meta = (ClampMin = 1))
	float SpawnRandomRate{ 1 };
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemSpawnRateData", meta = (ClampMin = 1))
	int SpawnRandomCount{ 1 };
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemSpawnRateData", meta = (ClampMin = 0.f, ClampMax = 1.0f))
	float SpawnPercent{ 0.0f };
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemSpawnRateData")
	int SpawnRangePrefixSum{ 0 };
};


USTRUCT(BlueprintType)
struct FItemSpawnListDataTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemSpawnListDataTableRow")
	TArray<FItemSpawnRateData> ItemSpawnList{};
};

UCLASS()
class PROJECTOPENWORLD_API UItemSpawnListAssetUserData : public UAssetUserData
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "ItemSpawnList")
	FName ItemSpawnDTListName{};
	UPROPERTY(EditAnywhere, Category = "ItemSpawnList")
	TArray<FItemSpawnRateData> ItemSpawnList{};
	UPROPERTY(EditAnywhere, Category = "ItemSpawnList", meta = (ClampMin = 1))
	int MaxSpawnCount = 1;

	UPROPERTY(EditAnywhere, Category = "ItemSpawnList")
	bool bEditItemSpawnListAsset{};
public:
	UFUNCTION()
	FItemSpawnRateData GetRandomItem() const;


	static FString StaticGetDataTablePath()
	{
		return TEXT("DataTable'/Game/Item/DataTable/DT_ItemSpawnListUserData.DT_ItemSpawnListUserData'");
	}
	int GetMaxSpawnCount() const { return MaxSpawnCount; }
private:

	virtual void PostEditChangeOwner() override;

};
