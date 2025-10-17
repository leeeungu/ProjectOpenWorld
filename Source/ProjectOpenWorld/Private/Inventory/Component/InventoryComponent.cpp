#include "Inventory/Component/InventoryComponent.h"
#include "Item/DataAsset/ItemPrimaryDataAsset.h"
#include "GameFramework/PlayerController.h"
#include "Player/Character/BasePlayer.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UInventoryComponent::AddItem(UItemPrimaryDataAsset* ItemData, int ItemCount)
{
	if (!ItemData || !maxInventoryWeight)
		return false;

	if (*maxInventoryWeight - (ItemData->GetItemWeight() * ItemCount) < totalInventoryWeight)
	{

	}

	float ItemWeights = ItemData->GetItemWeight() * ItemCount;
	totalInventoryWeight += ItemWeights;
	if (FInventorySlot* Slot = inventoryArray.FindByKey(FInventorySlot(ItemData)))
	{
		Slot->ItemCount+= ItemCount;
		Slot->ItemTotalWeights += ItemWeights;
	}
	else
	{
		FInventorySlot** EmptySlot = inventoryViewArray.FindByPredicate([](const FInventorySlot* Slot) { return Slot->ItemDataAsset == nullptr; });
		if (!EmptySlot)
			return false;
		(*EmptySlot)->ItemDataAsset = ItemData;
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

bool UInventoryComponent::SetInevntorySlot(int Row, int Col, UItemPrimaryDataAsset* ItemData, int ItemCount)
{
	int Index = Row * inventoryCol + Col;
	if (!inventoryViewArray.IsValidIndex(Index) )
		return false;
	FInventorySlot* SlotData = inventoryViewArray[Index];
	SlotData->ItemDataAsset = ItemData;
	SlotData->ItemCount = ItemCount;
	totalInventoryWeight -= SlotData->ItemTotalWeights;
	SlotData->ItemTotalWeights = 0;
	if (ItemData)
	{
		SlotData->ItemTotalWeights = ItemData->GetItemWeight() * SlotData->ItemCount;
	}
	SlotData->isEmpthySlot = ItemCount > 0;

	if (onUpdateInventory.IsBound())
	{
		onUpdateInventory.Broadcast();
	}
	return true;
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

void UInventoryComponent::UpdateInventoryWeight(float AddValue)
{
	//
	//if (*maxInventoryWeight - (ItemData->GetItemWeight() * ItemCount) < totalInventoryWeight)
	//{
	//
	//}
}

