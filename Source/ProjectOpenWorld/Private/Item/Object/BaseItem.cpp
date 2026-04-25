#include "Item/Object/BaseItem.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Item/DataAsset/ItemDataAsset.h"


EItemUseType UBaseItem::GetUseType() const
{
	UItemDataAsset* ItemDataAsset =	UItemDataSubsystem::GetPalItemDataAssetByName(GetItemID());
	if (ItemDataAsset)
	{
		return ItemDataAsset->GetItemUseType();
	}
	return EItemUseType();
}

UItemDataAsset* UBaseItem::GetPalItemDataAssetByName() const
{
	const FPalStaticItemDataStruct* Result{};
	UItemDataSubsystem::GetPalStaticItemDataPtr(ItemID, &Result);
	if(Result->ItemDataAssetSoft.IsValid())
		return Result->ItemDataAssetSoft.LoadSynchronous();
	return nullptr;
}