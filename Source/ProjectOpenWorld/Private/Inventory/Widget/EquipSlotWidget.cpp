#include "Inventory/Widget/EquipSlotWidget.h"
#include "Inventory/Component/InventoryComponent.h"

FReply UEquipSlotWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) )
	{
		if (UInventoryComponent* inventoryComponent = GetInventoryComponent())
		{
			FInventorySlot* pSrc = inventoryComponent->GetEmptyInventorySlot();
			inventoryComponent->SwapInventorySlot(pSrc, const_cast<FInventorySlot*>(itemPointer));
		}
	}
	return FReply::Handled();
}
