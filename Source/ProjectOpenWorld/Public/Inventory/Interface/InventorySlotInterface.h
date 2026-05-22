#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InventorySlotInterface.generated.h"

//class UItemPrimaryDataAsset;

struct FInventorySlot;

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

	virtual void SetSlotPtr(const FInventorySlot* Data) {}
	virtual const FInventorySlot* GetSlotDataPtr() const { return nullptr; }
};
