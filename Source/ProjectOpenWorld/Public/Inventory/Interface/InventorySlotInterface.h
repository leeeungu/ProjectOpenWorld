#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InventorySlotInterface.generated.h"

class UItemPrimaryDataAsset;

USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()
public:
	FInventorySlot() = default;

	FInventorySlot(UItemPrimaryDataAsset* DataAsset);


	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ItemData")
	TSoftObjectPtr<UItemPrimaryDataAsset> ItemDataAsset{};
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ItemData")
	int ItemCount{};
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ItemData")
	float TotalWeight{};

	bool operator==(const FInventorySlot& sDst);
	bool operator==(const UItemPrimaryDataAsset* pDst);
};

UINTERFACE(MinimalAPI)
class UInventorySlotInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTOPENWORLD_API IInventorySlotInterface
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	void SetSlotData(const FInventorySlot& Data);

};
