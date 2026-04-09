#include "Inventory/Component/InventoryComponent.h"
#include "GameFramework/PlayerController.h"
#include "Player/Character/BasePlayer.h"
#include "Item/DataTable/PalStaticItemDataStruct.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Item/Object/BaseItem.h"
#include "Item/Component/ItemUseComponent.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

UBaseItem* UInventoryComponent::CreateItemObject(FName ItemID, int32 ItemCount) const
{
	if (ItemID.IsNone() || ItemCount <= 0)
		return nullptr;

	UBaseItem* NewItem = NewObject<UBaseItem>(const_cast<UInventoryComponent*>(this));
	if (!NewItem)
		return nullptr;

	NewItem->SetItemID(ItemID);
	NewItem->SetItemCount(ItemCount);

	// 데이터 테이블/에셋에 UseType 필드가 있으면 여기서 세팅
	// NewItem->SetUseType(ItemDataStruct->UseType);

	return NewItem;
}

bool UInventoryComponent::AddItem(FName ItemID, int ItemCount)
{
	if (!UItemDataSubsystem::IsValidInstance() || ItemID.IsNone() || ItemCount <= 0)
		return false;

	const FPalStaticItemDataStruct* ItemDataStruct{};
	UItemDataSubsystem::GetPalStaticItemDataPtr(ItemID, ItemDataStruct);
	if (!ItemDataStruct)
		return false;

	const float AddedWeight = ItemDataStruct->Weight * ItemCount;
	if (maxInventoryWeight && (totalInventoryWeight + AddedWeight > *maxInventoryWeight))
		return false;

	if (FInventorySlot* Slot = inventoryArray.FindByPredicate(
		[ItemID](const FInventorySlot& InSlot)
		{
			return InSlot.ItemObject && InSlot.ItemObject->GetItemID() == ItemID;
		}))
	{
		Slot->ItemObject->SetItemCount(Slot->ItemObject->GetItemCount() + ItemCount);
		//Slot->SyncCachedData(ItemDataStruct->Weight);
	}
	else
	{
		FInventorySlot** EmptySlot = inventoryViewArray.FindByPredicate(
			[](const FInventorySlot* Slot)
			{
				return Slot && !Slot->ItemObject;
			});

		if (!EmptySlot)
			return false;

		UBaseItem* NewItem = CreateItemObject(ItemID, ItemCount);
		if (!NewItem)
			return false;
		(*EmptySlot)->ItemID = ItemID;
		(*EmptySlot)->ItemCount = ItemCount;
		(*EmptySlot)->isEmpthySlot = false;
		(*EmptySlot)->ItemObject = NewItem;
		//(*EmptySlot)->SyncCachedData(ItemDataStruct->Weight);
	}

	totalInventoryWeight += AddedWeight;
	if (PlayerCharacter)
		PlayerCharacter->UpdateWeight(totalInventoryWeight);

	if (onUpdateInventory.IsBound())
		onUpdateInventory.Broadcast();

	return true;
}

bool UInventoryComponent::HasItem(FName SearchItemID, int SearchItemCount) const
{
	if (SearchItemID.IsNone() || SearchItemCount <= 0)
		return false;

	int32 TotalCount = 0;
	for (const FInventorySlot& Slot : inventoryArray)
	{
		if (Slot.ItemObject && Slot.ItemObject->GetItemID() == SearchItemID)
		{
			TotalCount += Slot.ItemObject->GetItemCount();
			if (TotalCount >= SearchItemCount)
				return true;
		}
	}
	return false;
}

bool UInventoryComponent::RemoveItem(FName RemoveItemID, int RemoveItemCount)
{
	if (!UItemDataSubsystem::IsValidInstance() || RemoveItemID.IsNone() || RemoveItemCount <= 0)
		return false;

	const FPalStaticItemDataStruct* ItemDataStruct{};
	UItemDataSubsystem::GetPalStaticItemDataPtr(RemoveItemID, ItemDataStruct);
	if (!ItemDataStruct)
		return false;

	int32 RemainingCount = RemoveItemCount;
	int32 RemovedCount = 0;

	for (FInventorySlot& Slot : inventoryArray)
	{
		if (!Slot.ItemObject || Slot.ItemObject->GetItemID() != RemoveItemID)
			continue;

		const int32 CurrentCount = Slot.ItemObject->GetItemCount();
		const int32 RemoveCountFromThisSlot = FMath::Min(CurrentCount, RemainingCount);

		Slot.ItemObject->SetItemCount(CurrentCount - RemoveCountFromThisSlot);
		RemainingCount -= RemoveCountFromThisSlot;
		RemovedCount += RemoveCountFromThisSlot;

		if (Slot.ItemObject->GetItemCount() <= 0)
		{
			Slot.Clear();
		}
		else
		{
			//Slot.SyncCachedData(ItemDataStruct->Weight);
		}

		if (RemainingCount <= 0)
			break;
	}

	if (RemovedCount <= 0)
		return false;

	totalInventoryWeight -= ItemDataStruct->Weight * RemovedCount;
	totalInventoryWeight = FMath::Max(0.f, totalInventoryWeight);

	if (PlayerCharacter)
		PlayerCharacter->UpdateWeight(totalInventoryWeight);

	if (onUpdateInventory.IsBound())
		onUpdateInventory.Broadcast();

	return RemainingCount == 0;
}

bool UInventoryComponent::RemoveItemSlot(int Row, int Col, int RemoveItemCount)
{
	const int32 Index = Row * inventoryCol + Col;
	if (!inventoryViewArray.IsValidIndex(Index) || RemoveItemCount <= 0)
		return false;

	FInventorySlot* SlotData = inventoryViewArray[Index];
	if (!SlotData || !SlotData->ItemObject)
		return false;

	const FName ItemID = SlotData->ItemObject->GetItemID();

	const FPalStaticItemDataStruct* ItemDataStruct{};
	UItemDataSubsystem::GetPalStaticItemDataPtr(ItemID, ItemDataStruct);
	if (!ItemDataStruct)
		return false;

	const int32 CurrentCount = SlotData->ItemObject->GetItemCount();
	if (CurrentCount < RemoveItemCount)
		return false;

	SlotData->ItemObject->SetItemCount(CurrentCount - RemoveItemCount);
	totalInventoryWeight -= ItemDataStruct->Weight * RemoveItemCount;
	totalInventoryWeight = FMath::Max(0.f, totalInventoryWeight);

	if (PlayerCharacter)
		PlayerCharacter->UpdateWeight(totalInventoryWeight);

	if (SlotData->ItemObject->GetItemCount() <= 0)
	{
		SlotData->Clear();
	}
	else
	{
		//SlotData->SyncCachedData(ItemDataStruct->Weight);
	}

	if (onUpdateInventory.IsBound())
		onUpdateInventory.Broadcast();

	return true;
}

bool UInventoryComponent::DeleteItem(int Row, int Col)
{
	const int32 Index = Row * inventoryCol + Col;
	if (!inventoryViewArray.IsValidIndex(Index))
		return false;

	FInventorySlot* SlotData = inventoryViewArray[Index];
	if (!SlotData || !SlotData->ItemObject)
		return false;

	const FName ItemID = SlotData->ItemObject->GetItemID();
	const FPalStaticItemDataStruct* ItemDataStruct{};
	UItemDataSubsystem::GetPalStaticItemDataPtr(ItemID, ItemDataStruct);
	if (!ItemDataStruct)
		return false;

	totalInventoryWeight -= ItemDataStruct->Weight * SlotData->ItemObject->GetItemCount();
	totalInventoryWeight = FMath::Max(0.f, totalInventoryWeight);

	if (PlayerCharacter)
		PlayerCharacter->UpdateWeight(totalInventoryWeight);

	SlotData->Clear();

	if (onUpdateInventory.IsBound())
		onUpdateInventory.Broadcast();

	return true;
}

int UInventoryComponent::GetItemCount(FName SearchItemID) const
{
	int32 TotalCount = 0;
	for (const FInventorySlot& Slot : inventoryArray)
	{
		if (Slot.ItemObject && Slot.ItemObject->GetItemID() == SearchItemID)
		{
			TotalCount += Slot.ItemObject->GetItemCount();
		}
	}
	return TotalCount;
}

void UInventoryComponent::UseItem(int Row, int Col)
{
	const int32 Index = Row * inventoryCol + Col;
	if (!inventoryViewArray.IsValidIndex(Index))
		return;

	FInventorySlot* SlotData = inventoryViewArray[Index];
	if (!SlotData || !SlotData->ItemObject || !PlayerCharacter.IsValid())
		return;

	UItemUseComponent* ItemUseComponent = PlayerCharacter->GetPlayerItemUseComponent();
	if (!ItemUseComponent)
		return;

	if (ItemUseComponent->UseItem(SlotData->ItemObject))
	{
		RemoveItemSlot(Row, Col, 1);
	}
}

bool UInventoryComponent::SwapSlot(int SrcRow, int SrcCol, int DstRow, int DstCol)
{
	const int32 SrcIndex = SrcRow * inventoryCol + SrcCol;
	const int32 DstIndex = DstRow * inventoryCol + DstCol;

	if (!inventoryViewArray.IsValidIndex(SrcIndex) || !inventoryViewArray.IsValidIndex(DstIndex))
		return false;

	inventoryViewArray.Swap(SrcIndex, DstIndex);

	if (onUpdateInventory.IsBound())
		onUpdateInventory.Broadcast();

	return true;
}

bool UInventoryComponent::GetInventorySlotData(int Row, int Col, const FInventorySlot*& SlotData)
{
	const int32 Index = Row * inventoryCol + Col;
	if (!inventoryViewArray.IsValidIndex(Index))
		return false;

	SlotData = inventoryViewArray[Index];
	return true;
}

bool UInventoryComponent::GetSlotData(int Row, int Col, FInventorySlot& SlotData) const
{
	const int32 Index = Row * inventoryCol + Col;
	if (!inventoryViewArray.IsValidIndex(Index))
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

	for (int32 i = 0; i < inventorySize; ++i)
	{
		inventoryViewArray[i] = &inventoryArray[i];
	}

	APlayerController* Controller = Cast<APlayerController>(GetOwner());
	if (!Controller)
		return;

	if (PlayerCharacter = Cast<ABasePlayer>(Controller->GetPawn()))
	{
		maxInventoryWeight = PlayerCharacter->GetStatusRef(EStatusType::MaxWeight);
	}
}