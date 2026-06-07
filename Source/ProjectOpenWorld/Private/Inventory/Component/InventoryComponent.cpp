#include "Inventory/Component/InventoryComponent.h"
#include "GameFramework/PlayerController.h"
#include "Player/Character/BasePlayer.h"
#include "Item/DataTable/PalStaticItemDataStruct.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Item/FunctionLibrary/ItemFunctionLibrary.h"
#include "Item/Object/BaseItem.h"
#include "Item/Component/ItemUseComponent.h"
#include "Item/DataAsset/ItemDataAsset.h"
#include "Item/Object/Fragment/ItemDataSlotFragment.h"
#include "Player/Component/PlayerEquipComponent.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::OnPreSave()
{
	auto Gather = [](const TArray<FInventorySlot>& From, TArray<FPalSlotSaveData>& Out)
		{
			Out.Reset();
			for (const FInventorySlot& S : From)
			{
				if (!S.ItemObject) continue;
				FPalSlotSaveData D;
				D.SlotIndex = S.SlotIndex;
				D.ItemID = S.GetItemID();
				D.Count = S.GetItemCount();
				D.SlotType = static_cast<uint8>(S.SlotType);
				Out.Add(D);
			}
		};
	Gather(inventoryArray, SavedInventory);
	Gather(EquipSlot, SavedEquip);
}

void UInventoryComponent::OnLoaded()
{
	for (FInventorySlot& S : inventoryArray)
		S.Clear();
	for (FInventorySlot& S : EquipSlot)      
		S.Clear();
	totalInventoryWeight = 0.f;

	auto Restore = [this](TArray<FInventorySlot>& Arr, const TArray<FPalSlotSaveData>& From)
		{
			for (const FPalSlotSaveData& D : From)
			{
				UBaseItem* Item = UItemFunctionLibrary::CreateBaseItem(D.ItemID, D.Count, this);
				if (!SetItemAtSlot(Arr, D.SlotIndex, Item))   // 위치 보존 배치
					continue;
				if (const FPalStaticItemDataStruct* SD = FindStaticItemData(D.ItemID))
					totalInventoryWeight += SD->Weight * D.Count;
			}
		};
	Restore(inventoryArray, SavedInventory);
	Restore(EquipSlot, SavedEquip);

	BroadcastInventoryUpdated();   // 무게/UI 갱신
}

const FPalStaticItemDataStruct* UInventoryComponent::FindStaticItemData(FName ItemID) const
{
	if (!UItemDataSubsystem::IsValidInstance() || ItemID.IsNone())
		return nullptr;

	const FPalStaticItemDataStruct* ItemDataStruct = nullptr;
	UItemDataSubsystem::GetPalStaticItemDataPtr(ItemID, &ItemDataStruct);
	return ItemDataStruct;
}


void UInventoryComponent::BroadcastInventoryUpdated()
{
	if (onUpdateInventory.IsBound())
	{
		onUpdateInventory.Broadcast();
	}
	if (OnUpdateInventory_V2.IsBound())
	{
		OnUpdateInventory_V2.Broadcast(inventoryArray);
	}
	if (OnUpdateEquip.IsBound())
	{
		OnUpdateEquip.Broadcast(EquipSlot);
	}
}

bool UInventoryComponent::CheckStackable(const UBaseItem* Item) const
{
	if (!Item)
		return false;
	const FPalItemSlotData* SlotData{};
	if (!UItemDataSubsystem::GetPalItemSlotDataPtr(Item->GetItemID(), &SlotData))
		return false;
	return SlotData->bStackable;
}

bool UInventoryComponent::CanStackItem(const UBaseItem* Lhs, const UBaseItem* Rhs) const
{
	if (!Lhs || !Rhs)
		return false;

	if (Lhs->GetItemID() != Rhs->GetItemID())
		return false;

	return CheckStackable(Lhs);
}

bool UInventoryComponent::AddItem(UBaseItem* NewItem)
{
	if (!NewItem || NewItem->GetItemID().IsNone() || NewItem->GetItemCount() <= 0)
		return false;
	FName NewItemID = NewItem->GetItemID();
	int32 NewItemCount = NewItem->GetItemCount();
	const FPalStaticItemDataStruct* ItemDataStruct = FindStaticItemData(NewItemID);
	if (!ItemDataStruct)
		return false;
	const FPalItemSlotData* SlotData{};
	if (!UItemDataSubsystem::GetPalItemSlotDataPtr(NewItemID, &SlotData))
		return false;

	const float AddedWeight = ItemDataStruct->Weight * NewItem->GetItemCount();
	if (maxInventoryWeight && (totalInventoryWeight + AddedWeight > *maxInventoryWeight))
		return false;

	FInventorySlot* pStackSlot{};
	if (CheckStackable(NewItem))
	{
		for (FInventorySlot& Slot : inventoryArray)
		{
			if (Slot.GetItemID()== NewItemID && SlotData->MaxStackCount - NewItemCount > Slot.GetItemCount())
			{
				pStackSlot = &Slot;
				Slot.ItemObject->SetItemCount(Slot.ItemObject->GetItemCount() + NewItemCount);
				totalInventoryWeight += AddedWeight;
				BroadcastInventoryUpdated();
				return true;
			}
		}
	}
	if (FInventorySlot* EmptySlot = GetEmptyInventorySlot())
	{
		// 3. 인벤토리 소유 object로 복제
		UBaseItem* StoredItem = DuplicateObject<UBaseItem>(NewItem, this);
		if (!StoredItem)
			return false;

		EmptySlot->ItemObject = StoredItem;

		totalInventoryWeight += AddedWeight;
		BroadcastInventoryUpdated();
		return true;
	}
	return false;
}

bool UInventoryComponent::AddItem(FName ItemID, int ItemCount)
{
	UBaseItem* TempItem = UItemFunctionLibrary::CreateBaseItem(ItemID, ItemCount, PlayerCharacter.Get());// CreateItemObject(ItemID, ItemCount);
	if (!TempItem)
		return false;

	return AddItem(TempItem);
}

bool UInventoryComponent::SetItemAtSlot(TArray<FInventorySlot>& Arr, int32 SlotIndex, UBaseItem* Item)
{
	if (!Arr.IsValidIndex(SlotIndex) || !Item)
		return false;
	Arr[SlotIndex].ItemObject = Item;
	return true;
}

bool UInventoryComponent::ReturnItemToInventory(UBaseItem* BaseItem)
{
	if (!BaseItem || BaseItem->GetItemID().IsNone() || BaseItem->GetItemCount() <= 0)
		return false;

	const FPalStaticItemDataStruct* ItemDataStruct = FindStaticItemData(BaseItem->GetItemID());
	if (!ItemDataStruct)
		return false;

	FInventorySlot* EmptySlot{};
	for (FInventorySlot& Slot : inventoryArray)
	{
		if (!EmptySlot && !Slot.ItemObject)
		{
			EmptySlot = &Slot;
		}
		if (!Slot.ItemObject)
			continue;

		if (!CanStackItem(Slot.ItemObject, BaseItem))
			continue;

		Slot.ItemObject->SetItemCount(Slot.ItemObject->GetItemCount() + BaseItem->GetItemCount());

		BroadcastInventoryUpdated();
		return true;
	}

	if (!EmptySlot)
		return false;

	UBaseItem* StoredItem = DuplicateObject<UBaseItem>(BaseItem, this);
	if (!StoredItem)
		return false;

	EmptySlot->ItemObject = StoredItem;

	BroadcastInventoryUpdated();
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
	if (!inventoryArray.IsValidIndex(Index) || RemoveItemCount <= 0)
		return false;

	FInventorySlot* SlotData = &inventoryArray[Index];
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
	BroadcastInventoryUpdated();
	return true;
}

bool UInventoryComponent::RemoveItem(const FInventorySlot* Slot, int RemoveItemCount)
{
	if (!Slot || !inventoryArray.IsValidIndex(Slot->SlotIndex) || !Slot->ItemObject)
		return false;
	int Row = Slot->SlotIndex / inventoryCol;
	int Col = Slot->SlotIndex % inventoryCol;
	return RemoveItemSlot(Row, Col, RemoveItemCount);
}

bool UInventoryComponent::DeleteItem(int Row, int Col)
{
	const int32 Index = Row * inventoryCol + Col;
	if (!inventoryArray.IsValidIndex(Index))
		return false;

	FInventorySlot* SlotData = &inventoryArray[Index];
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
	if (!inventoryArray.IsValidIndex(Index) || ExtractCount <= 0)
		return nullptr;

	FInventorySlot* SlotData = &inventoryArray[Index];
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
	if (!inventoryArray.IsValidIndex(Index))
		return;

	FInventorySlot* SlotData = &inventoryArray[Index];
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

bool UInventoryComponent::UseItemSlot(const FInventorySlot* Slot)
{
	if (!Slot || !PlayerCharacter || !Slot->ItemObject || !inventoryArray.IsValidIndex(Slot->SlotIndex))
		return false;
	UItemUseComponent* ItemUseComponent = PlayerCharacter->GetPlayerItemUseComponent();
	if (ItemUseComponent)
	{
		return ItemUseComponent->UseItem(Slot->ItemObject);
	}
	return false;
}

bool UInventoryComponent::IsInventorySlot(const FInventorySlot* Slot) const
{
	if (!Slot)
		return false;
	return inventoryArray.IsValidIndex(Slot->SlotIndex);
}

bool UInventoryComponent::SwapSlot(int SrcRow, int SrcCol, int DstRow, int DstCol)
{
	const int32 SrcIndex = SrcRow * inventoryCol + SrcCol;
	const int32 DstIndex = DstRow * inventoryCol + DstCol;

	if (!inventoryArray.IsValidIndex(SrcIndex) || !inventoryArray.IsValidIndex(DstIndex))
		return false;
	FInventorySlot* SrcSlot = &inventoryArray[SrcIndex];
	FInventorySlot* DstSlot = &inventoryArray[DstIndex];

	return SwapInventorySlot(SrcSlot, DstSlot);
}

FInventorySlot* UInventoryComponent::GetInventorySlotData(int Row, int Col)
{
	const int32 Index = Row * inventoryCol + Col;
	if (!inventoryArray.IsValidIndex(Index))
		return nullptr;
	return &inventoryArray[Index];
}

bool UInventoryComponent::GetSlotData(int Row, int Col, FInventorySlot& SlotData) const
{
	const int32 Index = Row * inventoryCol + Col;
	if (!inventoryArray.IsValidIndex(Index)) 
		return false;

	SlotData = inventoryArray[Index];
	return true;
}

bool UInventoryComponent::SwapInventorySlot(FInventorySlot* Src, FInventorySlot* Dst)
{
	if (Src == Dst || !Src || !Dst)
		return false;
	// Dst의 아이템 타입이 Src의 타입이랑 맞으면 교환 가능
	EItemSlotType DstSlotType = EItemSlotType::None;
	bool bHaseFragment{};
	if (Dst->ItemObject)
	{
		if (UItemDataSlotFragment* DstSlotFragment = Cast< UItemDataSlotFragment>(Dst->ItemObject->GetItemDataFragment(UItemDataSlotFragment::StaticClass())))
		{
			DstSlotType = DstSlotFragment->GetSlotType();
			bHaseFragment = true;
		}
	}
	
	if (Src->Swap(Dst))
	{
		if (onUpdateInventory.IsBound())
		{
			onUpdateInventory.Broadcast();
		}
		return true;
	}
	return false;
}

FInventorySlot* UInventoryComponent::GetEmptyInventorySlot()
{
	for (FInventorySlot& Slot : inventoryArray)
	{
		if (!Slot.ItemObject)
		{
			return &Slot;
		}
	}
	return nullptr;
}

FInventorySlot* UInventoryComponent::GetStackableInventorySlot(const UBaseItem* NewItem)
{
	if (!NewItem)
		return nullptr;
	if (!CheckStackable(NewItem))
		return nullptr;
	FName NewItemID = NewItem->GetItemID();
	for (FInventorySlot& Slot : inventoryArray)
	{
		if (Slot.GetItemID() == NewItemID)
		{
			return &Slot;
		}
	}
	return nullptr;
}

const FInventorySlot* UInventoryComponent::GetEquipSlot(EItemSlotType SlotType) const
{
	if (EquipSlot.IsValidIndex(static_cast<uint8>(SlotType)))
	{
		return &EquipSlot[static_cast<uint8>(SlotType)];
	}
	return nullptr;
}

bool UInventoryComponent::SwapEquipSlot(const FInventorySlot* SrcEquip, const FInventorySlot* DstInventory) 
{
	if (!SrcEquip || !DstInventory)
		return false;
	FInventorySlot* Src = &EquipSlot[SrcEquip->SlotIndex];
	FInventorySlot* Dst = &inventoryArray[DstInventory->SlotIndex];
	if (!Src || !Dst)
		return false;
	UBaseItem* TempSrcItem = Src->ItemObject;
	Src->ItemObject = Dst->ItemObject;
	Dst->ItemObject = TempSrcItem;
	BroadcastInventoryUpdated();
	return true;
}

bool UInventoryComponent::SetEquipSlot(EItemSlotType SlotType, UBaseItem* NewItem)
{
	if (!NewItem || NewItem->GetItemID().IsNone() || NewItem->GetItemCount() <= 0 || !EquipSlot.IsValidIndex(static_cast<uint8>(SlotType)))
		return false;
	FInventorySlot* SrcEquip = &EquipSlot[static_cast<uint8>(SlotType)];
	FName NewItemID = NewItem->GetItemID();
	int32 NewItemCount = NewItem->GetItemCount();
	const FPalStaticItemDataStruct* ItemDataStruct = FindStaticItemData(NewItemID);
	if (!ItemDataStruct)
		return false;

	const float AddedWeight = ItemDataStruct->Weight * NewItem->GetItemCount();
	if (maxInventoryWeight && (totalInventoryWeight + AddedWeight > *maxInventoryWeight))
		return false;

	UBaseItem* StoredItem = DuplicateObject<UBaseItem>(NewItem, this);
	if (StoredItem)
	{
		SrcEquip->ItemObject = StoredItem;
		totalInventoryWeight += AddedWeight;
		BroadcastInventoryUpdated();
		return true;
	}
	return false;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	inventorySize = inventoryRow * inventoryCol;
	inventoryArray.Init(FInventorySlot(), inventorySize);
	for(int i =0 ; i < inventorySize; ++i)
	{
		inventoryArray[i].SlotType = EItemSlotType::None;
		inventoryArray[i].SlotIndex = i;
	}
	uint8 SlotTypeMax = static_cast<uint8>(EItemSlotType::SlotTypeEnumMax);
	EquipSlot.Init(FInventorySlot(), SlotTypeMax);
	for (int i = 0; i < SlotTypeMax; ++i)
	{
		EquipSlot[i].SlotType = static_cast<EItemSlotType>(i);
		EquipSlot[i].SlotIndex = i;
	}

	if (ABasePlayer* Player = Cast<ABasePlayer>(GetOwner()))
	{
		PlayerCharacter = Player;
		//maxInventoryWeight = Player->GetStatusRef(EStatusType::MaxWeight);
	}
	else if (APlayerController* Controller = Cast<APlayerController>(GetOwner()))
	{
		if (ABasePlayer* PawnPlayer = Cast<ABasePlayer>(Controller->GetPawn()))
		{
			PlayerCharacter = PawnPlayer;
			//maxInventoryWeight = PawnPlayer->GetStatusRef(EStatusType::MaxWeight);
		}
	}
}