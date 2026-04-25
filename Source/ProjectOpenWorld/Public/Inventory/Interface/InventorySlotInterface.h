#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InventorySlotInterface.generated.h"

//class UItemPrimaryDataAsset;
class UBaseItem;
enum class EItemSlotType : uint8;

USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()
public:
	FInventorySlot() = default;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ItemData")
	TObjectPtr<UBaseItem> ItemObject{};

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ItemData")
	int32 SlotIndex = -1;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Inventory")
	EItemSlotType SlotType{};

	bool operator==(const FInventorySlot& sDst) const;
	FName GetItemID() const;
	int32 GetItemCount() const;
	void Clear();
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

	virtual void SetSlotPtr(const FInventorySlot* Data) {}
	virtual const FInventorySlot* GetSlotDataPtr() const { return nullptr; }
};
