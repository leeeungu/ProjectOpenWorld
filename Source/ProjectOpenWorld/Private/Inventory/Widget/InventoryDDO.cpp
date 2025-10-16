#include "Inventory/Widget/InventoryDDO.h"

void UInventoryDDO::SetSlotData_Implementation(const FInventorySlot& Data)
{
	SlotData = &Data;
}

void UInventoryDDO::SetSlotIndex_Implementation(int Row, int Col)
{
	SlotRow = Row;
	SlotCol = Col;
}
