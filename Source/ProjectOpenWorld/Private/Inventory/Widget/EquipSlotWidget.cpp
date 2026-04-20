#include "Inventory/Widget/EquipSlotWidget.h"
#include "Inventory/Component/InventoryComponent.h"
#include "GameBase/Subsystem/UIDataGameInstanceSubsystem.h"

FReply UEquipSlotWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) )
	{
		if (UInventoryComponent* inventoryComponent = GetInventoryComponent())
		{
		
		}
	}
	return FReply::Handled();
}
