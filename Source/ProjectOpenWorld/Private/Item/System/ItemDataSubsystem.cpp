#include "Item/System/ItemDataSubsystem.h"
#include "Item/DataTable/WeaponeData.h"
#include "Item/DataAsset/ItemDataAsset.h"
#include "Item/Actor/ItemActor.h"

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

bool UItemDataSubsystem::GetPalItemMeshDataPtr(FName RowName, const FPalItemMeshData*& Data)
{
	if (const FPalItemMeshData* const* FoundData = SingletonInstance->PalItemMeshDataTableStruct.ItemDataMap.Find(RowName))
	{
		Data = *FoundData;
		return true;
	}
	Data = &SingletonInstance->PalItemMeshDataTableStruct.Dummy;
	return false;
}

bool UItemDataSubsystem::GetPalItemSlotDataPtr(FName RowName, const FPalItemSlotData*& Data)
{
	if (const FPalItemSlotData* const* FoundData = SingletonInstance->PalItemSlotDataTableStruct.ItemDataMap.Find(RowName))
	{
		Data = *FoundData;
		return true;
	}
	Data = &SingletonInstance->PalItemSlotDataTableStruct.Dummy;
	return false;
}

int32 UItemDataSubsystem::GetPalItemSlotDataMaxStackCountByName(FName RowName)
{
	const FPalItemSlotData* Result{};
	if (GetPalItemSlotDataPtr(RowName, Result))
		return Result->MaxStackCount;
	return int32();
}

FPalItemMeshData UItemDataSubsystem::GetPalItemMeshDataByName(FName RowName)
{
	const FPalItemMeshData* Result{};
	GetPalItemMeshDataPtr(RowName, Result);
	if (Result)
		return *Result;
	return FPalItemMeshData();
}

UStaticMesh* UItemDataSubsystem::GetItemMeshByName(FName RowName)
{
	const FPalItemMeshData* MeshData{};
	if (GetPalItemMeshDataPtr(RowName, MeshData) && MeshData->ItemStaticMeshSoft)
	{
		return MeshData->ItemStaticMeshSoft.LoadSynchronous();
	}
	return nullptr;
}

UTexture2D* UItemDataSubsystem::GetWeaponUI(EWeapone WeaponeType)
{
	if (SingletonInstance)
	{
		TArray<FWeaponUIData*> WeaponUIDataArray{};
		// /Script/Engine.DataTable'/Game/Item/DataTable/DT_WeaponUI.DT_WeaponUI'
		UDataTable* WeaponUIDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Item/DataTable/DT_WeaponUI.DT_WeaponUI"));
		if (WeaponUIDataTable)
		{
			uint8 Index = static_cast<uint8>(WeaponeType);
			WeaponUIDataTable->GetAllRows<FWeaponUIData>(TEXT(""), WeaponUIDataArray);
			return WeaponUIDataArray[Index]->WeaponeImage;
		}
	}
	return nullptr;
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

	if (!LoadAndSaveDataTableToMap(PalItemMeshDataTableStruct, TEXT("/Game/Item/DataTable/DT_ItemMesh.DT_ItemMesh")))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load DT_ItemMesh DataTable"));
	}
	///Script/Engine.DataTable'/Game/Item/DataTable/DT_PalItemSlotData.DT_PalItemSlotData'
	if (!LoadAndSaveDataTableToMap(PalItemSlotDataTableStruct, TEXT("/Game/Item/DataTable/DT_PalItemSlotData.DT_PalItemSlotData")))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load DT_PalItemSlotData DataTable"));
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

TSubclassOf<AItemActor> UItemDataSubsystem::GetPalStaticItemVisualBlueprintClassSoftByName(FName RowName) 
{
	const FPalStaticItemDataStruct* Result{};
	GetPalStaticItemDataPtr(RowName, Result);
	if (Result && Result->VisualBlueprintClassSoft)
		return TSubclassOf<AItemActor>(Result->VisualBlueprintClassSoft);
	return TSubclassOf<AItemActor>{};
}

TSubclassOf<UBaseItemObject> UItemDataSubsystem::GetPalStaticItemObjectVisualBlueprintClassSoftByName(FName RowName)
{
	const FPalStaticItemDataStruct* Result{};
	GetPalStaticItemDataPtr(RowName, Result);
	if (Result && Result->VisualBlueprintClassSoft)
		return TSubclassOf<UBaseItemObject>(Result->VisualBlueprintClassSoft);
	return TSubclassOf<UBaseItemObject>();
}

AItemActor* UItemDataSubsystem::SpawnPalStaticItemVisualActorByName(UObject* WorldContextObject, FName ItemID, const FTransform& SpawnTransform, int Count)
{
	if (WorldContextObject == nullptr)
		return nullptr;
	const FPalStaticItemDataStruct* Result{};
	GetPalStaticItemDataPtr(ItemID, Result);
	UWorld* World = WorldContextObject->GetWorld();
	TSubclassOf<UObject> SpawnClass = Result->VisualBlueprintClassSoft;
	if (World && Result->VisualBlueprintClassSoft == nullptr)
	{
		// Script / Engine.Blueprint'/Game/Item/Blueprint/Base/BP_BaseItem.BP_BaseItem'
		SpawnClass = LoadClass<UObject>(World, TEXT("/Game/Item/Blueprint/Base/BP_BaseItem.BP_BaseItem_C"));
	}
	if (World && SpawnClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		AItemActor* SpawnedActor = World->SpawnActor<AItemActor>(SpawnClass, SpawnTransform, SpawnParams);
		if (SpawnedActor)
		{
			SpawnedActor->Init(ItemID, Count);
		}
		return SpawnedActor;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UItemDataSubsystem::Failed to spawn item actor. World or SpawnClass is null."));
	}
	return nullptr;
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

UItemDataAsset* UItemDataSubsystem::GetPalItemDataAssetByName(FName RowName)
{
	const FPalStaticItemDataStruct* Result{};
	GetPalStaticItemDataPtr(RowName, Result);
	return Result->ItemDataAssetSoft.LoadSynchronous();
}

UTexture2D* UItemDataSubsystem::GetPalItemIconTextureByName(FName RowName) 
{
	const FPalEditorItemIconTableRow* Result{};
	GetPalItemIconDataPtr(RowName, Result);
	if (Result)
		return Result->Icon;
	return nullptr;
}
