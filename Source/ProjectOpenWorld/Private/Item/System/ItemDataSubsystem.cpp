#include "Item/System/ItemDataSubsystem.h"

	
UItemDataSubsystem* UItemDataSubsystem::SingletonInstance{};

bool UItemDataSubsystem::GetPalStaticItemDataPtr(FName RowName, const FPalStaticItemDataStruct*& Data) 
{
	if (!SingletonInstance)
		return false;
	if (const FPalStaticItemDataStruct* const* FoundData = SingletonInstance->PalStaticItemDataTableStruct.ItemDataMap.Find(RowName))
	{
		Data = *FoundData;
		return true;
	}
	Data = &SingletonInstance->PalStaticItemDataTableStruct.Dummy;
	return false;
}

bool UItemDataSubsystem::GetPalItemRecipeDataPtr(FName RowName, const FPalItemRecipe*& Data)
{
	if (const FPalItemRecipe* const* FoundData = SingletonInstance->PalItemRecipeDataTableStruct.ItemDataMap.Find(RowName))
	{
		Data = *FoundData;
		return true;
	}
	Data = &SingletonInstance->PalItemRecipeDataTableStruct.Dummy;
	return false;
}

bool UItemDataSubsystem::GetPalItemIconDataPtr(FName RowName, const FPalEditorItemIconTableRow*& Data)
{
	if (const FPalEditorItemIconTableRow* const* FoundData = SingletonInstance->PalItemIconDataTableStruct.ItemDataMap.Find(RowName))
	{
		Data = *FoundData;
		return true;
	}
	Data = &SingletonInstance->PalItemIconDataTableStruct.Dummy;
	return false;
}


void UItemDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	SingletonInstance = this;
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

FPalStaticItemDataStruct UItemDataSubsystem::GetPalStaticItemDataByName(FName RowName) 
{
	const FPalStaticItemDataStruct* Result{};
	GetPalStaticItemDataPtr(RowName, Result);
	if (Result)
		return *Result;
	return FPalStaticItemDataStruct{};
}

FPalItemRecipe UItemDataSubsystem::GetPalItemRecipeDataByName(FName RowName)
{
	const FPalItemRecipe* Result{};
	GetPalItemRecipeDataPtr(RowName, Result);
	if (Result)
		return *Result;
	return FPalItemRecipe();
}

FString UItemDataSubsystem::GetPalStaticItemOverrideNameByName(FName RowName) 
{
	const FPalStaticItemDataStruct* Result{};
	GetPalStaticItemDataPtr(RowName, Result);
	if (Result)
		return Result->OverrideName;
	return FString{};
}

FString UItemDataSubsystem::GetPalStaticItemOverrideDescriptionByName(FName RowName) 
{
	const FPalStaticItemDataStruct* Result{};
	GetPalStaticItemDataPtr(RowName, Result); 
	if (Result)
		return Result->OverrideDescription;
	return FString{};
}

FString UItemDataSubsystem::GetPalStaticItemIconNameByName(FName RowName) 
{
	const FPalStaticItemDataStruct* Result{};
	GetPalStaticItemDataPtr(RowName, Result); 
	if (Result)
		return Result->IconName;
	return FString{};
}

int32 UItemDataSubsystem::GetPalStaticItemMaxStackCountByName(FName RowName) 
{
	const FPalStaticItemDataStruct* Result{};
	GetPalStaticItemDataPtr(RowName, Result); 
	if (Result)
		return Result->MaxStackCount;
	return -1;
}

float UItemDataSubsystem::GetPalStaticItemWeightByName(FName RowName) 
{
	const FPalStaticItemDataStruct* Result{};
	GetPalStaticItemDataPtr(RowName, Result);
	if (Result)
		return Result->Weight;
	return 0.0f;
}

int32 UItemDataSubsystem::GetPalStaticItemPriceByName(FName RowName) 
{
	const FPalStaticItemDataStruct* Result{};
	GetPalStaticItemDataPtr(RowName, Result);
	if (Result)
		return Result->Price;
	return -1;
}

int32 UItemDataSubsystem::GetPalStaticItemSortIDByName(FName RowName) 
{
	const FPalStaticItemDataStruct* Result{};
	GetPalStaticItemDataPtr(RowName, Result);
	if (Result)
		return Result->SortID;
	return 99999;
}

TSoftClassPtr<AItemActor> UItemDataSubsystem::GetPalStaticItemVisualBlueprintClassSoftByName(FName RowName) 
{
	const FPalStaticItemDataStruct* Result{};
	GetPalStaticItemDataPtr(RowName, Result);
	if (Result)
		return Result->VisualBlueprintClassSoft;
	return TSoftClassPtr<AItemActor>{};
}

FString UItemDataSubsystem::GetPalItemRecipeProductIdByName(FName RowName) 
{
	const FPalItemRecipe* Result{};
	GetPalItemRecipeDataPtr(RowName, Result);
	if (Result)
		return Result->Product_Id;
	return FString{};
}

int32 UItemDataSubsystem::GetPalItemRecipeProductCountByName(FName RowName) 
{
	const FPalItemRecipe* Result{};
	GetPalItemRecipeDataPtr(RowName, Result);
	if (Result)
		return Result->Product_Count;
	return -1;
}

float UItemDataSubsystem::GetPalItemRecipeWorkAmountByName(FName RowName)
{
	const FPalItemRecipe* Result{};
	GetPalItemRecipeDataPtr(RowName, Result);
	if (Result)
		return Result->WorkAmount;
	return 0.0f;
}

FString UItemDataSubsystem::GetPalItemRecipeUnlockItemIDByName(FName RowName)
{
	const FPalItemRecipe* Result{};
	GetPalItemRecipeDataPtr(RowName, Result);
	if (Result)
		return Result->UnlockItemID;
	return FString{};
}

const TArray<FRecipeMaterialData>& UItemDataSubsystem::GetPalItemRecipeMaterialsByName(FName RowName)
{
	const FPalItemRecipe* Result{};
	GetPalItemRecipeDataPtr(RowName, Result);
	return Result->Materials;
}

UTexture2D* UItemDataSubsystem::GetPalItemIconTextureByName(FName RowName) 
{
	const FPalEditorItemIconTableRow* Result{};
	GetPalItemIconDataPtr(RowName, Result);
	if (Result)
		return Result->Icon;
	return nullptr;
}
