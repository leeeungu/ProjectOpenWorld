#include "Inventory/Widget/EquipSlotWidget.h"
#include "Inventory/Component/InventoryComponent.h"
#include "GameBase/Subsystem/UIDataGameInstanceSubsystem.h"
#include "Inventory/Widget/InventoryDDO.h"

void UEquipSlotWidget::OnEquipSlotDrop(UDragDropOperation* Operation)
{

}

FReply UEquipSlotWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) )
	{
		if (UInventoryComponent* inventoryComponent = GetInventoryComponent())
		{
			if (itemPointer && itemPointer->ItemObject)
			{
				if (inventoryComponent->UnUseItemSlot(itemPointer))
				{
					inventoryComponent->ReturnItemToInventory(itemPointer->ItemObject);
				}
			}
		}
	}
	return FReply::Handled();
}

bool UEquipSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (UInventoryDDO* DDO = Cast< UInventoryDDO>(InOperation))
	{
		if (UInventoryComponent* inventoryComponent = GetInventoryComponent())
		{
			if (itemPointer && itemPointer->ItemObject)
			{
				if (inventoryComponent->UnUseItemSlot(itemPointer))
				{ 
					UE_LOG(LogTemp, Warning, TEXT("UEquipSlotWidget::NativeOnDrop UnUseItemSlot"));
					inventoryComponent->ReturnItemToInventory(itemPointer->ItemObject);
				}
			}
			if (const  FInventorySlot* slotData = DDO->GetSlotDataPtr())
			{
				UE_LOG(LogTemp, Warning, TEXT("UEquipSlotWidget::NativeOnDrop"));
				inventoryComponent->UseItemSlot(slotData);
			}
		}
		return SwapSlot(Cast< UInventorySlotBase>(DDO->Payload));
	}
	return UUserWidget::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);;
}
