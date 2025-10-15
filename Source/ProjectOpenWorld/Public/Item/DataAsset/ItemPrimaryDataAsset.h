#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemPrimaryDataAsset.generated.h"

class UInventorySlotWidget;
UCLASS()
class PROJECTOPENWORLD_API UItemPrimaryDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemData")
	FName ItemName{};
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemData", meta = (ClampMin = 0.0f))
	float ItemWeight{};

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemData")
	TSubclassOf<UInventorySlotWidget> InventorySlotWidget{};
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemData")
	TSoftObjectPtr<UTexture2D> ItemSlotImage{};
public:
	UItemPrimaryDataAsset();
	UFUNCTION(BlueprintPure, Category = "ItemData")
	float GetItemWeight() const { return ItemWeight; }
	UFUNCTION(BlueprintPure, Category = "ItemData")
	FName GetItemName() const { return ItemName; }
	UFUNCTION(BlueprintPure, Category = "InventoryData")
	TSubclassOf<UInventorySlotWidget> GetInventorySlotWidgetClass() const { return InventorySlotWidget; }

	UFUNCTION(BlueprintPure, Category = "ItemData")
	TSoftObjectPtr<UTexture2D> GetItemSlotImage() const { return ItemSlotImage; }

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
