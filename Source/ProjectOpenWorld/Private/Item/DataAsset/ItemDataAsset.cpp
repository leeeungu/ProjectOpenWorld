#include "Item/DataAsset/ItemDataAsset.h"

const TArray<TObjectPtr<UItemDataFragment>> UItemDataAsset::GetItemDataFragmentArrayOfClass(TSubclassOf<UItemDataFragment> InUserDataClass)
{
	TArray<TObjectPtr<UItemDataFragment>> Result{};
	for (int32 DataIdx = 0; DataIdx < ItemDataFragments.Num(); DataIdx++)
	{
		TObjectPtr<UItemDataFragment> Datum = ItemDataFragments[DataIdx];
		if (Datum != NULL && Datum->IsA(InUserDataClass))
		{
			Result.Add(Datum);
		}
	}
	return Result;
}

const TObjectPtr<UItemDataFragment> UItemDataAsset::GetItemDataFragmentOfClass(TSubclassOf<UItemDataFragment> InUserDataClass)
{
	for (int32 DataIdx = 0; DataIdx < ItemDataFragments.Num(); DataIdx++)
	{
		TObjectPtr<UItemDataFragment> Datum = ItemDataFragments[DataIdx];
		if (Datum != NULL && Datum->IsA(InUserDataClass))
		{
			return Datum;
		}
	}
	return nullptr;
}
	

const TArray<TObjectPtr<UItemDataFragment>>* UItemDataAsset::GetItemDataFragmentArray()
{
	return &ItemDataFragments;
	/*
#if WITH_EDITOR
	if (IsRunningCookCommandlet())
	{
		return &ToRawPtrTArrayUnsafe(ItemDataFragments);
	}
	else
	{
		static thread_local TArray<TObjectPtr<UItemDataFragment>> CachedItemDataFragments;
		CachedItemDataFragments.Reset();
		CachedItemDataFragments.Append(ItemDataFragments);
		return &ToRawPtrTArrayUnsafe(CachedItemDataFragments);
	}
#else
	return &ToRawPtrTArrayUnsafe(ItemDataFragments);
#endif*/
}
