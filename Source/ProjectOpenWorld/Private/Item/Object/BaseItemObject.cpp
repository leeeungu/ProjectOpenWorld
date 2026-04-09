#include "Item/Object/BaseItemObject.h"
#include "Item/Object/BaseItem.h"


FName UBaseItemObject::GetItemID() const
{
	if (ItemDataObject)
	{
		return ItemDataObject->GetItemID();
	}
	return NAME_None;
}