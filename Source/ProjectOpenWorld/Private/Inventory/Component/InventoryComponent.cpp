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
		inventoryArray.Push(FInventorySlot(ItemData));
	}

	return true;
}

bool UInventoryComponent::GetInventorySlotData(int Row, int Col, const FInventorySlot*  SlotData)
{
	int Index = Row * inventoryCol + Col;
	if (!inventoryArray.IsValidIndex(Index))
	{
		return false;
	}
	SlotData = &inventoryArray[Index];
	return true;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

