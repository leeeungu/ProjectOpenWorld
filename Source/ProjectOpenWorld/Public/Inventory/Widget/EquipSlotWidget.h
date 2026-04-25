#pragma once

#include "CoreMinimal.h"
#include "Inventory/Widget/InventorySlotBase.h"
#include "EquipSlotWidget.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UEquipSlotWidget : public UInventorySlotBase
{
	GENERATED_BODY()
protected:
	UFUNCTION()
	void OnEquipSlotDrop(UDragDropOperation* Operation);
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
