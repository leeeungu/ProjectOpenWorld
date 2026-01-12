#include "Inventory/Interface/InventorySlotInterface.h"
#include "Item/DataAsset/ItemPrimaryDataAsset.h"

FInventorySlot::FInventorySlot(FName NewItemID)
	: ItemCount{} ,ItemTotalWeights{}
{
	ItemID = NewItemID;
	ItemCount = 1;
}
//
//FInventorySlot::FInventorySlot(UItemPrimaryDataAsset* DataAsset) :
//	 ItemCount{}, ItemTotalWeights{}
//{
//	//ItemDataAsset = DataAsset;
//	ItemCount = 1;
//	//if (DataAsset)
//	//	ItemTotalWeights = DataAsset->GetItemWeight() * ItemCount;
//}

bool FInventorySlot::operator==(const FInventorySlot& sDst)
{
	return ItemID == sDst.ItemID;
}

