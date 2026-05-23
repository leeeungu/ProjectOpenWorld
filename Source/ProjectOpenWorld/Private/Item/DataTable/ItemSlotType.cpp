#include "Item/DataTable/ItemSlotType.h"

bool FInventorySlot::Swap(FInventorySlot* Dst)
{
	if (!Dst)
		return false;
	if (SlotType == EItemSlotType::None || Dst->SlotType == SlotType)
	{
		UBaseItem* TempSrcItem = ItemObject;
		ItemObject = Dst->ItemObject;
		Dst->ItemObject = TempSrcItem;
		return true;
	}
	return false;
}

void FInventorySlot::Clear()
{
	ItemObject = nullptr;
}

bool FInventorySlot::operator==(const FInventorySlot& sDst) const
{
	return GetItemID() == sDst.GetItemID();
}