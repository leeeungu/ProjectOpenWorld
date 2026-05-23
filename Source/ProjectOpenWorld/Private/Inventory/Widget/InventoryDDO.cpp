#include "Inventory/Widget/InventoryDDO.h"
#include "Item/DataTable/ItemSlotType.h"

void UInventoryDDO::SetSlotData_Implementation(const FInventorySlot& Data)
{
	SlotData = &Data;
}

void UInventoryDDO::SetSlotIndex_Implementation(int Row, int Col)
{
	SlotRow = Row;
	SlotCol = Col;
}

FInventorySlot UInventoryDDO::GetSlotData() const
{
	return *SlotData;
}
