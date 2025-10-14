#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemPrimaryDataAsset.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UItemPrimaryDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemData", meta = (ClampMin = 0.0f))
	FName ItemName{};
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemData", meta = (ClampMin = 0.0f))
	float ItemWeight{};

public:
	UFUNCTION(BlueprintPure)
	float GetItemWeight() const { return ItemWeight; }
	UFUNCTION(BlueprintPure)
	FName GetItemName() const { return ItemName; }

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
