#include "Item/FunctionLibrary/ItemFunctionLibrary.h"

UBaseItem* UItemFunctionLibrary::CreateBaseItem(FName ItemId, int32 ItemCount,UObject* Outer, TSubclassOf<UBaseItem> ItemClass)
{
	if (!ItemClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateBaseItem Failed : ItemClass is null"));
		return nullptr;
	}

	UBaseItem* NewItem = NewObject<UBaseItem>(Outer, ItemClass);
	if (!NewItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateBaseItem Failed : NewObject failed"));
		return nullptr;
	}

	NewItem->SetItemID(ItemId);
	NewItem->SetItemCount(ItemCount);

	return NewItem;
}

UBaseItemObject* UItemFunctionLibrary::CreateBaseItemObject(UObject* Outer, FName ItemId, int32 ItemCount, TSubclassOf<UBaseItemObject> ItemObjectClass, TSubclassOf<UBaseItem> ItemClass)
{
	if (!ItemObjectClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateBaseItemObject Failed : ItemObjectClass is null"));
		return nullptr;
	}

	if (!ItemClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateBaseItemObject Failed : ItemClass is null"));
		return nullptr;
	}

	UBaseItemObject* NewItemObject = NewObject<UBaseItemObject>(GetTransientPackage(), ItemObjectClass);
	if (!IsValid(NewItemObject))
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateBaseItemObject Failed : NewObject<UBaseItemObject> failed"));
		return nullptr;
	}

	// Outer를 ItemObject로 줘서 소유 관계를 일단 맞춘다.
	UBaseItem* NewItem = CreateBaseItem(ItemId, ItemCount, NewItemObject, ItemClass);
	if (!IsValid(NewItem))
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateBaseItemObject Failed : NewObject<UBaseItem> failed"));
		return nullptr;
	}
	NewItemObject->SetBaseItem(NewItem);
	return NewItemObject;
}
