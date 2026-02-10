#include "Item/DataTable/PalDropItemDatabaseRow.h"

FPalDropItemDatabaseRow::FPalDropItemDatabaseRow() : Super()
{
	DropItemList.Init({},GetDropItemListSize());
}
