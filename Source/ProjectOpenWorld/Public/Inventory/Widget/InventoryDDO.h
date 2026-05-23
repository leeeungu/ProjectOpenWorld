#pragma once

#include "CoreMinimal.h"
#include "Inventory/Interface/InventorySlotInterface.h"
#include "Blueprint/DragDropOperation.h"
#include "InventoryDDO.generated.h"

struct FInventorySlot;
UCLASS()
class PROJECTOPENWORLD_API UInventoryDDO : public UDragDropOperation, public IInventorySlotInterface
{
	GENERATED_BODY()
protected:
	const FInventorySlot* SlotData{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "InventoryDDO", meta = (ExposeOnSpawn = "true"))
	int SlotRow{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "InventoryDDO", meta = (ExposeOnSpawn = "true"))
	int SlotCol{};

public:

	virtual void SetSlotData_Implementation(const FInventorySlot& Data) override;
	virtual void SetSlotIndex_Implementation(int Row, int Col) override;

	virtual void SetSlotPtr(const FInventorySlot* Data) override { SlotData = Data; }
	virtual const FInventorySlot* GetSlotDataPtr() const override { return SlotData; }

	UFUNCTION(BlueprintPure, Category = "InventoryDDO")
	FInventorySlot GetSlotData() const;
	UFUNCTION(BlueprintPure, Category = "InventoryDDO")
	int GetSlotRow() const { return SlotRow; }
	UFUNCTION(BlueprintPure, Category = "InventoryDDO")
	int GetSlotCol() const { return SlotCol; }
};
