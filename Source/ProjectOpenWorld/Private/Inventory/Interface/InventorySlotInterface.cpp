#include "Inventory/Interface/InventorySlotInterface.h"
#include "Item/Object/BaseItem.h"
#include "Item/DataTable/ItemSlotType.h"

FName FInventorySlot::GetItemID() const
{
	return ItemObject ? ItemObject->GetItemID() : FName{};
}

int32 FInventorySlot::GetItemCount() const
{
	return ItemObject ? ItemObject->GetItemCount() : 0;
}

void FInventorySlot::Clear()
{
	ItemObject = nullptr;
}

bool FInventorySlot::operator==(const FInventorySlot& sDst) const
{
	return GetItemID() == sDst.GetItemID();
}