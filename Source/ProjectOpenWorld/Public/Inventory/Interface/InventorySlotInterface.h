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

	FInventorySlot(FName NewItemID);
	//FInventorySlot(UItemPrimaryDataAsset* DataAsset);


	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ItemData")
	FName ItemID{};
	//UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ItemData")
	//TObjectPtr<UItemPrimaryDataAsset> ItemDataAsset{};
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ItemData")
	int ItemCount{};
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ItemData")
	float ItemTotalWeights{};

	bool isEmpthySlot = true;

	bool operator==(const FInventorySlot& sDst);
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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	void SetSlotIndex(int Row, int Col);
};
