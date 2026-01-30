#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "ItemSpawnListAssetUserData.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UItemSpawnListAssetUserData : public UAssetUserData
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "ItemSpawnList")
	TArray<FName> ItemSpawnList{};
	UPROPERTY(EditAnywhere, Category = "ItemSpawnList")
	TArray<float> ItemSpawnRateList{};
public:
	UFUNCTION()
	FName GetRandomItemSpawn() const;

};
