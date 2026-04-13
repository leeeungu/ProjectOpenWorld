#include "Inventory/Component/InventoryComponent.h"
#include "GameFramework/PlayerController.h"
#include "Player/Character/BasePlayer.h"
#include "Item/DataTable/PalStaticItemDataStruct.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Item/FunctionLibrary/ItemFunctionLibrary.h"
#include "Item/Object/BaseItem.h"
#include "Item/Component/ItemUseComponent.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

const FPalStaticItemDataStruct* UInventoryComponent::FindStaticItemData(FName ItemID) const
{
	if (!UItemDataSubsystem::IsValidInstance() || ItemID.IsNone())
		return nullptr;

	const FPalStaticItemDataStruct* ItemDataStruct = nullptr;
	UItemDataSubsystem::GetPalStaticItemDataPtr(ItemID, ItemDataStruct);
	return ItemDataStruct;
}

void UInventoryComponent::RefreshSlotCache(FInventorySlot& Slot)
{
	Slot.RefreshFromObject();
}

void UInventoryComponent::BroadcastInventoryUpdated()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->UpdateWeight(totalInventoryWeight);
	}

	if (onUpdateInventory.IsBound())
	{
		onUpdateInventory.Broadcast();
	}
}

bool UInventoryComponent::CanStackItem(const UBaseItem* Lhs, const UBaseItem* Rhs) const
{
	if (!Lhs || !Rhs)
		return false;

	// 현재 최소 조건
	if (Lhs->GetClass() != Rhs->GetClass())
		return false;

	if (Lhs->GetItemID() != Rhs->GetItemID())
		return false;

	const FPalItemSlotData* SlotData{};
	UItemDataSubsystem::GetPalItemSlotDataPtr(Lhs->GetItemID(), SlotData);
	if (!SlotData)
		return false;
	
	if (!SlotData->bStackable)
		return false;
	
	if (SlotData->MaxStackCount <= 1)
		return false;

	return true;
}

bool UInventoryComponent::AddItem(UBaseItem* NewItem)
{
	if (!NewItem || NewItem->GetItemID().IsNone() || NewItem->GetItemCount() <= 0)
		return false;

	const FPalStaticItemDataStruct* ItemDataStruct = FindStaticItemData(NewItem->GetItemID());
	if (!ItemDataStruct)
		return false;

	const float AddedWeight = ItemDataStruct->Weight * NewItem->GetItemCount();
	if (maxInventoryWeight && (totalInventoryWeight + AddedWeight > *maxInventoryWeight))
		return false;

	// 1. 스택 가능한 기존 슬롯 탐색
	for (FInventorySlot& Slot : inventoryArray)
	{
		if (!Slot.ItemObject)
			continue;

		if (!CanStackItem(Slot.ItemObject, NewItem))
			continue;

		Slot.ItemObject->SetItemCount(Slot.ItemObject->GetItemCount() + NewItem->GetItemCount());
		RefreshSlotCache(Slot);

		totalInventoryWeight += AddedWeight;
		BroadcastInventoryUpdated();
		return true;
	}

	// 2. 빈 슬롯 탐색
	FInventorySlot** EmptySlot = inventoryViewArray.FindByPredicate(
		[](const FInventorySlot* Slot)
		{
			return Slot && Slot->ItemObject == nullptr;
		});

	if (!EmptySlot)
		return false;

	// 3. 인벤토리 소유 object로 복제
	UBaseItem* StoredItem = DuplicateObject<UBaseItem>(NewItem, this);
	if (!StoredItem)
		return false;

	(*EmptySlot)->ItemObject = StoredItem;
	RefreshSlotCache(**EmptySlot);

	totalInventoryWeight += AddedWeight;
	BroadcastInventoryUpdated();
	return true;
}

bool UInventoryComponent::AddItem(FName ItemID, int ItemCount)
{
	UBaseItem* TempItem = UItemFunctionLibrary::CreateBaseItem(ItemID, ItemCount, PlayerCharacter.Get());// CreateItemObject(ItemID, ItemCount);
	if (!TempItem)
		return false;

	return AddItem(TempItem);
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
	if (RemoveItemID.IsNone() || RemoveItemCount <= 0)
		return false;

	const FPalStaticItemDataStruct* ItemDataStruct = FindStaticItemData(RemoveItemID);
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
			RefreshSlotCache(Slot);
		}

		if (RemainingCount <= 0)
			break;
	}

	if (RemovedCount <= 0)
		return false;

	totalInventoryWeight -= ItemDataStruct->Weight * RemovedCount;
	totalInventoryWeight = FMath::Max(0.f, totalInventoryWeight);
	BroadcastInventoryUpdated();

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
	const FPalStaticItemDataStruct* ItemDataStruct = FindStaticItemData(ItemID);
	if (!ItemDataStruct)
		return false;

	const int32 CurrentCount = SlotData->ItemObject->GetItemCount();
	if (CurrentCount < RemoveItemCount)
		return false;

	SlotData->ItemObject->SetItemCount(CurrentCount - RemoveItemCount);

	totalInventoryWeight -= ItemDataStruct->Weight * RemoveItemCount;
	totalInventoryWeight = FMath::Max(0.f, totalInventoryWeight);

	if (SlotData->ItemObject->GetItemCount() <= 0)
	{
		SlotData->Clear();
	}
	else
	{
		RefreshSlotCache(*SlotData);
	}

	BroadcastInventoryUpdated();
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

	const FPalStaticItemDataStruct* ItemDataStruct = FindStaticItemData(SlotData->ItemObject->GetItemID());
	if (!ItemDataStruct)
		return false;

	totalInventoryWeight -= ItemDataStruct->Weight * SlotData->ItemObject->GetItemCount();
	totalInventoryWeight = FMath::Max(0.f, totalInventoryWeight);

	SlotData->Clear();
	BroadcastInventoryUpdated();
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

UBaseItem* UInventoryComponent::ExtractItemObject(int Row, int Col, int ExtractCount, UObject* NewOuter)
{
	const int32 Index = Row * inventoryCol + Col;
	if (!inventoryViewArray.IsValidIndex(Index) || ExtractCount <= 0)
		return nullptr;

	FInventorySlot* SlotData = inventoryViewArray[Index];
	if (!SlotData || !SlotData->ItemObject)
		return nullptr;

	const int32 CurrentCount = SlotData->ItemObject->GetItemCount();
	if (CurrentCount < ExtractCount)
		return nullptr;

	UObject* TargetOuter = NewOuter ? NewOuter : GetTransientPackage();

	UBaseItem* OutItem = DuplicateObject<UBaseItem>(SlotData->ItemObject, TargetOuter);
	if (!OutItem)
		return nullptr;

	OutItem->SetItemCount(ExtractCount);

	if (!RemoveItemSlot(Row, Col, ExtractCount))
	{
		return nullptr;
	}

	return OutItem;
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
		// TODO:
		// 장비/비소모품은 제거하면 안 된다.
		// 이후 UseResult 구조체나 ConsumeCount 반환 형태로 바꾸는 게 맞다.
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
	{
		onUpdateInventory.Broadcast();
	}
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

	if (ABasePlayer* Player = Cast<ABasePlayer>(GetOwner()))
	{
		PlayerCharacter = Player;
		maxInventoryWeight = Player->GetStatusRef(EStatusType::MaxWeight);
	}
	else if (APlayerController* Controller = Cast<APlayerController>(GetOwner()))
	{
		if (ABasePlayer* PawnPlayer = Cast<ABasePlayer>(Controller->GetPawn()))
		{
			PlayerCharacter = PawnPlayer;
			maxInventoryWeight = PawnPlayer->GetStatusRef(EStatusType::MaxWeight);
		}
	}
}