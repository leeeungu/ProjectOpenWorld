#include "Item/System/ItemDataSubsystem.h"

bool UItemDataSubsystem::GetPalStaticItemDataPtr(FName RowName, const FPalStaticItemDataStruct*& Data) const
{
	if (const FPalStaticItemDataStruct* const* FoundData = PalStaticItemDataTableStruct.ItemDataMap.Find(RowName))
	{
		Data = *FoundData;
		return true;
	}
	Data = &PalStaticItemDataTableStruct.Dummy;
	return false;
}

bool UItemDataSubsystem::GetPalItemRecipeDataPtr(FName RowName, const FPalItemRecipe*& Data) const
{
	if (const FPalItemRecipe* const* FoundData = PalItemRecipeDataTableStruct.ItemDataMap.Find(RowName))
	{
		Data = *FoundData;
		return true;
	}
	Data = &PalItemRecipeDataTableStruct.Dummy;
	return false;
}

bool UItemDataSubsystem::GetPalItemIconDataPtr(FName RowName, const FPalEditorItemIconTableRow*& Data) const
{
	if (const FPalEditorItemIconTableRow* const* FoundData = PalItemIconDataTableStruct.ItemDataMap.Find(RowName))
	{
		Data = *FoundData;
		return true;
	}
	Data = &PalItemIconDataTableStruct.Dummy;
	return false;
}


void UItemDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	//Script/Engine.DataTable'/Game/Item/DataTable/DT_ItemDataTable.DT_ItemDataTable'
	if (!LoadAndSaveDataTableToMap(PalStaticItemDataTableStruct, TEXT("/Game/Item/DataTable/DT_PalStaticItemData.DT_PalStaticItemData")))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load PalStaticItemData DataTable"));
	}
	//Script/Engine.DataTable'/Game/Item/DataTable/DT_ItemRecipeDataTable.DT_ItemRecipeDataTable'
	if (!LoadAndSaveDataTableToMap(PalItemRecipeDataTableStruct, TEXT("/Game/Item/DataTable/DT_ItemRecipeDataTable.DT_ItemRecipeDataTable")))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load ItemRecipeData DataTable"));
	}
	// Script / Engine.DataTable'/Game/Item/DataTable/DT_ItemIconDataTable.DT_ItemIconDataTable'
	if (!LoadAndSaveDataTableToMap(PalItemIconDataTableStruct, TEXT("/Game/Item/DataTable/DT_ItemIconDataTable.DT_ItemIconDataTable")))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load ItemIconData DataTable"));
	}
}

FPalStaticItemDataStruct UItemDataSubsystem::GetPalStaticItemDataByName(FName RowName) const
{
	const FPalStaticItemDataStruct* Reulst{};
	GetPalStaticItemDataPtr(RowName, Reulst);
	return *Reulst;
}

FString UItemDataSubsystem::GetPalStaticItemOverrideNameByName(FName RowName) const
{
	const FPalStaticItemDataStruct* Reulst{};
	GetPalStaticItemDataPtr(RowName, Reulst);
	return Reulst->OverrideName;
}

FString UItemDataSubsystem::GetPalStaticItemOverrideDescriptionByName(FName RowName) const
{
	const FPalStaticItemDataStruct* Reulst{};
	GetPalStaticItemDataPtr(RowName, Reulst);
	return Reulst->OverrideDescription;
}

FString UItemDataSubsystem::GetPalStaticItemIconNameByName(FName RowName) const
{
	const FPalStaticItemDataStruct* Reulst{};
	GetPalStaticItemDataPtr(RowName, Reulst);
	return Reulst->IconName;
}

int32 UItemDataSubsystem::GetPalStaticItemMaxStackCountByName(FName RowName) const
{
	const FPalStaticItemDataStruct* Reulst{};
	GetPalStaticItemDataPtr(RowName, Reulst);
	return Reulst->MaxStackCount;
}

float UItemDataSubsystem::GetPalStaticItemWeightByName(FName RowName) const
{
	const FPalStaticItemDataStruct* Reulst{};
	GetPalStaticItemDataPtr(RowName, Reulst);
	return Reulst->Weight;
}

int32 UItemDataSubsystem::GetPalStaticItemPriceByName(FName RowName) const
{
	const FPalStaticItemDataStruct* Reulst{};
	GetPalStaticItemDataPtr(RowName, Reulst);
	return Reulst->Price;
}

int32 UItemDataSubsystem::GetPalStaticItemSortIDByName(FName RowName) const
{
	const FPalStaticItemDataStruct* Reulst{};
	GetPalStaticItemDataPtr(RowName, Reulst);
	return Reulst->SortID;
}

TSoftClassPtr<AItemActor> UItemDataSubsystem::GetPalStaticItemVisualBlueprintClassSoftByName(FName RowName) const
{
	const FPalStaticItemDataStruct* Reulst{};
	GetPalStaticItemDataPtr(RowName, Reulst);
	return Reulst->VisualBlueprintClassSoft;
}

FString UItemDataSubsystem::GetPalItemRecipeProductIdByName(FName RowName) const
{
	const FPalItemRecipe* Reulst{};
	GetPalItemRecipeDataPtr(RowName, Reulst);
	return Reulst->Product_Id;
}

int32 UItemDataSubsystem::GetPalItemRecipeProductCountByName(FName RowName) const
{
	const FPalItemRecipe* Reulst{};
	GetPalItemRecipeDataPtr(RowName, Reulst);
	return Reulst->Product_Count;
}

float UItemDataSubsystem::GetPalItemRecipeWorkAmountByName(FName RowName) const
{
	const FPalItemRecipe* Reulst{};
	GetPalItemRecipeDataPtr(RowName, Reulst);
	return Reulst->WorkAmount;
}

FString UItemDataSubsystem::GetPalItemRecipeUnlockItemIDByName(FName RowName) const
{
	const FPalItemRecipe* Reulst{};
	GetPalItemRecipeDataPtr(RowName, Reulst);
	return Reulst->UnlockItemID;
}

const TArray<FMaterialData>& UItemDataSubsystem::GetPalItemRecipeMaterialsByName(FName RowName) const
{
	const FPalItemRecipe* Reulst{};
	GetPalItemRecipeDataPtr(RowName, Reulst);
	return Reulst->Materials;
}

UTexture2D* UItemDataSubsystem::GetPalItemIconTextureByName(FName RowName) const
{
	const FPalEditorItemIconTableRow* Reulst{};
	GetPalItemIconDataPtr(RowName, Reulst);
	return Reulst->Icon;
}
