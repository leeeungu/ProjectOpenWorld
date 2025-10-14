#include "Inventory/Component/InventoryComponent.h"
#include "Item/DataAsset/ItemPrimaryDataAsset.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UInventoryComponent::AddItem(UItemPrimaryDataAsset* ItemData)
{
	if (!ItemData || maxInventoryWeight - ItemData->GetItemWeight() < totalInventoryWeight)
		return false;

	totalInventoryWeight += ItemData->GetItemWeight();
	if (FInventorySlot* Slot = inventoryArray.FindByKey(FInventorySlot(ItemData)))
	{
		Slot->ItemCount++;
		Slot->TotalWeight += ItemData->GetItemWeight();
	}
	else
	{
		FInventorySlot* EmptySlot = inventoryArray.FindByPredicate([](const FInventorySlot& Slot) { return Slot.ItemDataAsset == nullptr; });
		if (!EmptySlot)
			return false;
		EmptySlot->ItemDataAsset = ItemData;
		EmptySlot->ItemCount = 1;
		EmptySlot->TotalWeight = ItemData->GetItemWeight();
	}
	if (onUpdateInventory.IsBound())
	{
		onUpdateInventory.Broadcast();
	}
	return true;
}

bool UInventoryComponent::GetInventorySlotData(int Row, int Col, const FInventorySlot*&  SlotData)
{
	int Index = Row * inventoryCol + Col;
	if (!inventoryArray.IsValidIndex(Index))
		return false;
	SlotData = &inventoryArray[Index];
	return true;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	inventorySize = inventoryRow * inventoryCol;
	inventoryArray.Init(FInventorySlot(), inventorySize);
}

