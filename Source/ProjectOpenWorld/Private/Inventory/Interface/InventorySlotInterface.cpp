#include "Inventory/Interface/InventorySlotInterface.h"
#include "Item/DataAsset/ItemPrimaryDataAsset.h"

FInventorySlot::FInventorySlot(UItemPrimaryDataAsset* DataAsset) :
	ItemDataAsset{}, ItemCount{}, ItemTotalWeights{}
{
	ItemDataAsset = DataAsset;
	ItemCount = 1;
	if (DataAsset)
		ItemTotalWeights = DataAsset->GetItemWeight() * ItemCount;
}

bool FInventorySlot::operator==(const FInventorySlot& sDst)
{
	return ItemDataAsset == sDst.ItemDataAsset;
}

bool FInventorySlot::operator==(const UItemPrimaryDataAsset* pDst)
{
	return ItemDataAsset == pDst;
}
