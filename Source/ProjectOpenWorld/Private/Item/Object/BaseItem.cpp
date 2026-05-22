#include "Item/Object/BaseItem.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Item/DataAsset/ItemDataAsset.h"
#include "Item/Object/ItemDataFragment.h"
#include "Item/Object/Fragment/ItemDataSlotFragment.h"

EItemUseType UBaseItem::GetUseType() const
{
	UItemDataAsset* ItemDataAsset =	UItemDataSubsystem::GetPalItemDataAssetByName(GetItemID());
	if (ItemDataAsset)
	{
		return ItemDataAsset->GetItemUseType();
	}
	return EItemUseType();
}

int32 UBaseItem::AddItemCount(int32 Count)
{
	int32 AddValue = Count;
	if (Count > 0)
	{
		int32 Max = std::numeric_limits<int32>::max() - Count;
		if (Max <= ItemCount)
		{
			AddValue = std::numeric_limits<int32>::max() - ItemCount;
		}
	}
	else
	{
		if (ItemCount + Count <= 0)
		{
			AddValue = -ItemCount;
		}
	}
	ItemCount += AddValue;
	return AddValue;
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
