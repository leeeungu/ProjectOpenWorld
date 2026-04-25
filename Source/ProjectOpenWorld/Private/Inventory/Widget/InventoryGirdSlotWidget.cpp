#include "Inventory/Widget/InventoryGirdSlotWidget.h"
#include "Inventory/Component/InventoryComponent.h"
#include "GameBase/Subsystem/UIDataGameInstanceSubsystem.h"

FReply UInventoryGirdSlotWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) )
	{
		if (UInventoryComponent* inventoryComponent = GetInventoryComponent())
		{
			if (inventoryComponent->IsInventorySlot(itemPointer))
			{
				if (inventoryComponent->UseItemSlot(itemPointer))
				{
					inventoryComponent->RemoveItemSlot(inventoryRow, inventoryCol, 1);
				}
			}
		}
	}
	return FReply::Handled();
}
