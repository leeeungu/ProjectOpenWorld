#include "Inventory/Component/InventoryComponent.h"
#include "GameFramework/PlayerController.h"
#include "Player/Character/BasePlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Item/DataTable/PalStaticItemDataStruct.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Item/Object/BaseItemObject.h"
#include "Player/Component/PlayerItemComponent.h"
#include "GameBase/FunctionLib/StringTableFunctionLibrary.h"


UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UInventoryComponent::AddItem(FName ItemID, int ItemCount)
{
	if (!UItemDataSubsystem::IsValidInstance())
		return false;
	const FPalStaticItemDataStruct* ItemDataStruct{};
	UItemDataSubsystem::GetPalStaticItemDataPtr(ItemID, ItemDataStruct);
	if (!ItemDataStruct)
		return false;

	float ItemWeights = ItemDataStruct->Weight * ItemCount;
	if (maxInventoryWeight  && *maxInventoryWeight - ItemWeights < totalInventoryWeight)
		return false;
	totalInventoryWeight += ItemWeights;
	if (PlayerCharacter)
		PlayerCharacter->UpdateWeight(totalInventoryWeight);
	if (FInventorySlot* Slot = inventoryArray.FindByKey(FInventorySlot(ItemID)))
	{
		Slot->ItemID = ItemID;
		Slot->ItemCount += ItemCount;
		Slot->ItemTotalWeights += ItemWeights;
		Slot->isEmpthySlot = false;
	}
	else
	{
		FInventorySlot** EmptySlot = inventoryViewArray.FindByPredicate([](const FInventorySlot* Slot) { return Slot->ItemID == NAME_None; });
		if (!EmptySlot)
				return false;
		(*EmptySlot)->ItemID = ItemID;
		(*EmptySlot)->ItemCount = ItemCount;
		(*EmptySlot)->ItemTotalWeights = ItemWeights;
		(*EmptySlot)->isEmpthySlot = false;
	}
	if (onUpdateInventory.IsBound())
	{
		onUpdateInventory.Broadcast();
	}
	return true;
}

bool UInventoryComponent::HasItem(FName SearchItemID, int SearchItemCount) const
{
	int ToTalCount = 0;
	for (const FInventorySlot& Slot : inventoryArray)
	{
		if (Slot.ItemID == SearchItemID)
		{
			ToTalCount += Slot.ItemCount;
			if (ToTalCount >= SearchItemCount)
				return true;
		}
	}
	return false;
}

bool UInventoryComponent::RemoveItem(FName RemoveItemID, int RemoveItemCount)
{
	if (!UItemDataSubsystem::IsValidInstance())
		return false;
	const FPalStaticItemDataStruct* ItemDataStruct{};
	UItemDataSubsystem::GetPalStaticItemDataPtr(RemoveItemID, ItemDataStruct);
	if (!ItemDataStruct)
		return false;
	int RemainingCount = RemoveItemCount;
	for (FInventorySlot& Slot : inventoryArray)
	{
		if (Slot.ItemID == RemoveItemID)
		{
			if (Slot.ItemCount >= RemainingCount)
			{
				float ItemWeights = ItemDataStruct->Weight * RemainingCount;
				Slot.ItemCount -= RemainingCount;
				Slot.ItemTotalWeights -= ItemWeights;
				totalInventoryWeight -= ItemWeights;
				if (PlayerCharacter)
					PlayerCharacter->UpdateWeight(totalInventoryWeight);
				if (Slot.ItemCount <= 0)
				{
					Slot.ItemID = NAME_None;
					Slot.isEmpthySlot = true;
					Slot.ItemTotalWeights = 0;
				}
				if (onUpdateInventory.IsBound())
				{
					onUpdateInventory.Broadcast();
				}
				return true;
			}
			else
			{
				RemainingCount -= Slot.ItemCount;
				totalInventoryWeight -= Slot.ItemTotalWeights;
				Slot.ItemID = NAME_None;
				Slot.ItemCount = 0;
				Slot.isEmpthySlot = true;
				Slot.ItemTotalWeights = 0;
			}
		}
	}
	totalInventoryWeight -= ItemDataStruct->Weight * (RemoveItemCount - RemainingCount);
	if (PlayerCharacter)
		PlayerCharacter->UpdateWeight(totalInventoryWeight);
	if (onUpdateInventory.IsBound())
	{
		onUpdateInventory.Broadcast();
	}
	return false;
}

bool UInventoryComponent::RemoveItemSlot(int Row, int Col, int RemoveItemCount)
{
	int Index = Row * inventoryCol + Col;
	if(!inventoryViewArray.IsValidIndex(Index))
		return false;
	FInventorySlot* SlotData = inventoryViewArray[Index];
	if (!SlotData || SlotData->isEmpthySlot || SlotData->ItemCount < RemoveItemCount)
		return false;
	const FPalStaticItemDataStruct* ItemDataStruct{};
	UItemDataSubsystem::GetPalStaticItemDataPtr(SlotData->ItemID, ItemDataStruct);
	if (!ItemDataStruct)
		return false;
	SlotData->ItemCount -= RemoveItemCount;
	totalInventoryWeight -= ItemDataStruct->Weight;
	if (PlayerCharacter)
		PlayerCharacter->UpdateWeight(totalInventoryWeight);
	if (SlotData->ItemCount <= 0)
	{
		SlotData->ItemCount = 0;
		SlotData->isEmpthySlot = true;
		SlotData->ItemID = NAME_None;
	}
	if (onUpdateInventory.IsBound())
	{
		onUpdateInventory.Broadcast();
	}
	return true;
}

bool UInventoryComponent::DeleteItem(int Row, int Col)
{
	int Index = Row * inventoryCol + Col;
	if (!inventoryViewArray.IsValidIndex(Index))
		return false;
	FInventorySlot* SlotData = inventoryViewArray[Index];
	if (!SlotData->isEmpthySlot)
	{
		if (!UItemDataSubsystem::IsValidInstance())
			return false;
		const FPalStaticItemDataStruct* ItemDataStruct{};
		UItemDataSubsystem::GetPalStaticItemDataPtr(SlotData->ItemID, ItemDataStruct);
		if (!ItemDataStruct)
			return false;
		totalInventoryWeight -= SlotData->ItemTotalWeights;
		if (PlayerCharacter)
			PlayerCharacter->UpdateWeight(totalInventoryWeight);
		SlotData->ItemID = NAME_None;
		SlotData->ItemCount = 0;
		SlotData->ItemTotalWeights = 0;
		SlotData->isEmpthySlot = true;
		if (onUpdateInventory.IsBound())
		{
			onUpdateInventory.Broadcast();
		}
		return true;
	}
	return false;
}

int UInventoryComponent::GetItemCount(FName SearchItemID) const
{
	int ToTalCount = 0;
	for (const FInventorySlot& Slot : inventoryArray)
	{
		if (Slot.ItemID == SearchItemID)
		{
			ToTalCount += Slot.ItemCount;
		}
	}
	return ToTalCount;
}

void UInventoryComponent::UseItem(int Row, int Col)
{
	int Index = Row * inventoryCol + Col;
	if (!inventoryViewArray.IsValidIndex(Index))
		return;
	FInventorySlot* SlotData = inventoryViewArray[Index];
	if (SlotData->isEmpthySlot)
		return;
	TSubclassOf<UBaseItemObject> ItemObjectClass = UItemDataSubsystem::GetPalStaticItemObjectVisualBlueprintClassSoftByName(SlotData->ItemID);
	if (!ItemObjectClass)
	{
		ItemObjectClass = UItemDataSubsystem::GetPalStaticItemObjectVisualBlueprintClassSoftByName("Money");

		//FText ItemName = UStringTableFunctionLibrary::GetItemNameFromStringTable(SlotData->ItemID.ToString());
		//if (UStringTableFunctionLibrary::GetUnValidItem() == ItemName.ToString())
		//{
		//	
		//	//SlotData->ItemCount * 
		//	RemoveItemSlot(Row, Col, SlotData->ItemCount);
		//	//ItemObjectClass = UItemDataSubsystem::GetPalStaticItemObjectVisualBlueprintClassSoftByName(NAME_None);
		//}
	}
	else if (PlayerCharacter->GetPlayerItemComponent()->RegisterItemActor(ItemObjectClass))
	{
		RemoveItemSlot(Row, Col, 1);
	}
}

bool UInventoryComponent::SwapSlot(int SrcRow, int SrcCol, int DstRow, int DstCol)
{
	int SrcIndex = SrcRow * inventoryCol + SrcCol;
	int DstIndex = DstRow * inventoryCol + DstCol;
	if (!inventoryViewArray.IsValidIndex(SrcIndex) || !inventoryViewArray.IsValidIndex(DstIndex))
		return false;

	inventoryViewArray.Swap(SrcIndex, DstIndex);
	if (onUpdateInventory.IsBound())
	{
		onUpdateInventory.Broadcast();
	}
	return true;
}

bool UInventoryComponent::GetInventorySlotData(int Row, int Col, const FInventorySlot*&  SlotData)	
{
	int Index = Row * inventoryCol + Col;
	if (!inventoryArray.IsValidIndex(Index))
		return false;
	SlotData = inventoryViewArray[Index];
	return true;
}

bool UInventoryComponent::GetSlotData(int Row, int Col, FInventorySlot& SlotData) const
{
	int Index = Row * inventoryCol + Col;
	if (!inventoryArray.IsValidIndex(Index))
		return false;
	SlotData = *inventoryViewArray[Index];
	UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::GetSlotData Getting Slot Data: %s Count: %d"), *SlotData.ItemID.ToString(), SlotData.ItemCount);
	return true;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	inventorySize = inventoryRow * inventoryCol;
	inventoryArray.Init(FInventorySlot(), inventorySize);
	inventoryViewArray.Init(nullptr, inventorySize);

	for (int i = 0; i < inventorySize; i++)
	{
		inventoryViewArray[i] = &inventoryArray[i];
	}
	APlayerController* Controller = Cast<APlayerController>(GetOwner());
	if (!Controller)
		return;
	if(PlayerCharacter = Cast< ABasePlayer>(Controller->GetPawn()))
	{
		maxInventoryWeight = PlayerCharacter->GetStatusRef(EStatusType::MaxWeight);
	}
}


