#include "Inventory/Interface/InventorySlotInterface.h"
#include "Item/Object/BaseItem.h"

//FInventorySlot::FInventorySlot(FName NewItemID)
//	: ItemCount{} ,ItemTotalWeights{}
//{
//	ItemID = NewItemID;
//	ItemCount = 1;
//}
//
//FInventorySlot::FInventorySlot(UItemPrimaryDataAsset* DataAsset) :
//	 ItemCount{}, ItemTotalWeights{}
//{
//	//ItemDataAsset = DataAsset;
//	ItemCount = 1;
//	//if (DataAsset)
//	//	ItemTotalWeights = DataAsset->GetItemWeight() * ItemCount;
//}

bool FInventorySlot::operator==(const FInventorySlot& sDst) const
{
	return ItemID == sDst.ItemID;
}

void FInventorySlot::RefreshFromObject()
{
	if (ItemObject)
	{
		ItemID = ItemObject->GetItemID();
		ItemCount = ItemObject->GetItemCount();
		isEmpthySlot = false;
	}
	else
	{
		Clear();
	}
}

