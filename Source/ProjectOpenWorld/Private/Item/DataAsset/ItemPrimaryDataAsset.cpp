#include "Item/DataAsset/ItemPrimaryDataAsset.h"

FPrimaryAssetId UItemPrimaryDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("ItemDataAsset"), GetFName());
}
