#include "Item/Object/BaseItem.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Item/DataAsset/ItemDataAsset.h"
#include "Item/Object/ItemDataFragment.h"

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
	if(UItemDataSubsystem::GetPalStaticItemDataPtr(ItemID, &Result) && Result->ItemDataAssetSoft)
		return Result->ItemDataAssetSoft;
	return nullptr;
}

UItemDataFragment* UBaseItem::GetItemDataFragment(TSubclassOf<UItemDataFragment> FragClass) const
{
	if(FragClass)
		UE_LOG(LogTemp, Log, TEXT("GetItemDataFragment called with ItemID: %s and FragClass: %s"), *GetItemID().ToString(), *FragClass->GetName());
	UItemDataAsset* ItemDataAsset = GetPalItemDataAssetByName();
	if (ItemDataAsset)
	{
		return ItemDataAsset->GetItemDataFragmentOfClass(FragClass);
	}
	return nullptr;
}
