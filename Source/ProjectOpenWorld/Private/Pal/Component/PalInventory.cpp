#include "Pal/Component/PalInventory.h"
#include "Item/Object/BaseItem.h"

UPalInventory::UPalInventory() : Super{}
{
	PrimaryComponentTick.bCanEverTick = false;
	Slots.Init( FInventorySlot{}, InventorySize);
}

void UPalInventory::BeginPlay()
{
	Super::BeginPlay();
	Slots.Init( FInventorySlot{}, InventorySize);
	for (int32 i = 0; i < GetInventorySize(); i++)
	{
		Slots[i].SlotIndex = i;
	}
}

bool UPalInventory::FindSameItem(UBaseItem* NewItem, int32& Index) const
{
	if (!NewItem)
		return false;
	Index = 0;
	while(Slots.IsValidIndex(Index) && !(Slots[Index].ItemObject && Slots[Index].ItemObject->GetItemID() == NewItem->GetItemID()))
	{
		Index++;
	}
	return Slots.IsValidIndex(Index);
}

bool UPalInventory::FindEmptySlot(int32& Index) const
{
	Index = 0;
	while(Slots.IsValidIndex(Index) && Slots[Index].ItemObject)
	{
		Index++;
	}
	return Slots.IsValidIndex(Index);
}

bool UPalInventory::AddItem(UBaseItem* NewItem)
{
	int32 ResultIndex{};
	FInventorySlot* ResultSlot{};

	int32 SameIndex{};
	FInventorySlot* SameSlot{};
	if (FindSameItem(NewItem, SameIndex))
	{
		SameSlot = &Slots[SameIndex];
		SameSlot->ItemObject->AddItemCount(NewItem->GetItemCount());
		ResultIndex = SameIndex;
		ResultSlot = SameSlot;
	}

	int32 EmptyIndex{};
	FInventorySlot* EmptySlot{};
	if (FindEmptySlot(EmptyIndex) && !ResultSlot)
	{
		EmptySlot = &Slots[EmptyIndex];
		UBaseItem* StoredItem = DuplicateObject<UBaseItem>(NewItem, this);
		if (!StoredItem)
			return false;
		EmptySlot->ItemObject = StoredItem;
		ResultIndex = EmptyIndex;
		ResultSlot = EmptySlot;
	}
	if (ResultSlot && Slots.IsValidIndex(ResultIndex))
	{
		OnSlotChanged.Broadcast(ResultIndex, *ResultSlot);
	}
	return ResultSlot != nullptr;
}

bool UPalInventory::RemoveItem(int32 SlotIndex, int32 ItemCount)
{
	if (Slots.IsValidIndex(SlotIndex))
	{
		FInventorySlot* ResultSlot = &Slots[SlotIndex];
		if (ResultSlot && ResultSlot->ItemObject)
		{
			const int32 CurrentCount = ResultSlot->ItemObject->GetItemCount();
			const int32 RemoveCountFromThisSlot = FMath::Min(CurrentCount, ItemCount);
			const int32 NewCount = CurrentCount - RemoveCountFromThisSlot;
			ResultSlot->ItemObject->SetItemCount(NewCount);
			if (NewCount <= 0)
			{
				ResultSlot->Clear();
			}
			OnSlotChanged.Broadcast(SlotIndex, *ResultSlot);
			return true;
		}
	}
	return false;
}

bool UPalInventory::SwapSlots(int32 Src, int32 Dst)
{
	if (Slots.IsValidIndex(Src) && Slots.IsValidIndex(Dst) && Src != Dst)
	{
		Slots.Swap(Src, Dst);
		OnSlotChanged.Broadcast(Src, Slots[Src]);
		OnSlotChanged.Broadcast(Dst, Slots[Dst]);
		return true;
	}
	return false;
}

FInventorySlot UPalInventory::GetInventorySlot(int32 Index) const
{
	if (Slots.IsValidIndex(Index))
	{
		return Slots[Index];
	}
	return FInventorySlot{};
}

int32 UPalInventory::GetInventorySize() const
{
	return InventorySize;
}

void UPalInventory::OnOpenUI()
{
	for (int32 i = 0; i < GetInventorySize(); i++)
	{
		OnSlotChanged.Broadcast(i, Slots[i]);
	}
}

void UPalInventory::OnSwapSlots(int32 Src, int32 Dst)
{
	if (Slots.IsValidIndex(Src) && Slots.IsValidIndex(Dst) && Src != Dst)
	{
		Slots.Swap(Src, Dst);
		OnSlotChanged.Broadcast(Src, Slots[Src]);
		OnSlotChanged.Broadcast(Dst, Slots[Dst]);
	}
}
