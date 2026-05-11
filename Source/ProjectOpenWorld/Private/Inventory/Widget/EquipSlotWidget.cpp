#include "Inventory/Widget/EquipSlotWidget.h"
#include "Inventory/Component/InventoryComponent.h"
#include "Player/Character/BasePlayer.h"
#include "Item/Component/EquipmentComponent.h"


FReply UEquipSlotWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) )
	{
		if (UInventoryComponent* inventoryComponent = GetInventoryComponent())
		{
			FInventorySlot* pSrc = inventoryComponent->GetEmptyInventorySlot();
			UEquipmentComponent* equipComponent = GetPlayerEquipComponent();
			if (equipComponent)
			{
				equipComponent->UnRegisterItem(itemPointer->ItemObject);
			}
			inventoryComponent->SwapInventorySlot(pSrc, const_cast<FInventorySlot*>(itemPointer));
		}
	}
	return FReply::Handled();
}


UEquipmentComponent* UEquipSlotWidget::GetPlayerEquipComponent() const
{
	if (ABasePlayer* Player = Cast<ABasePlayer>(GetOwningPlayerPawn()))
	{
		return Player->GetPlayerEquipComponent();
	}
	return nullptr;
}
