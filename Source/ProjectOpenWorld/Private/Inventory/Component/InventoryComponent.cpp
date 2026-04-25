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

const FPalStaticItemDataStruct* UInventoryComponent::FindStaticItemData(FName ItemID) const
{
	if (!UItemDataSubsystem::IsValidInstance() || ItemID.IsNone())
		return nullptr;

	const FPalStaticItemDataStruct* ItemDataStruct = nullptr;
	UItemDataSubsystem::GetPalStaticItemDataPtr(ItemID, &ItemDataStruct);
	return ItemDataStruct;
}

void UInventoryComponent::RefreshSlotCache(FInventorySlot& Slot)
{
//Slot.RefreshFromObject();
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
	UItemDataSubsystem::GetPalItemSlotDataPtr(Lhs->GetItemID(), &SlotData);
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

	FInventorySlot* EmptySlot{};
	for (FInventorySlot& Slot : inventoryArray)
	{
		if (!EmptySlot && !Slot.ItemObject)
		{
			EmptySlot = &Slot;
		}
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

	if (!EmptySlot)
		return false;

	// 3. 인벤토리 소유 object로 복제
	UBaseItem* StoredItem = DuplicateObject<UBaseItem>(NewItem, this);
	if (!StoredItem)
		return false;

	EmptySlot->ItemObject = StoredItem;
	RefreshSlotCache(*EmptySlot);

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
		RefreshSlotCache(Slot);

		BroadcastInventoryUpdated();
		return true;
	}

	if (!EmptySlot)
		return false;

	UBaseItem* StoredItem = DuplicateObject<UBaseItem>(BaseItem, this);
	if (!StoredItem)
		return false;

	EmptySlot->ItemObject = StoredItem;
	RefreshSlotCache(*EmptySlot);

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
	else
	{
		RefreshSlotCache(*SlotData);
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

bool UInventoryComponent::UnUseItemSlot(const FInventorySlot* pSrc)
{
	if (!pSrc || !pSrc->ItemObject || !PlayerCharacter.IsValid())
		return false;
	UItemUseComponent* ItemUseComponent = PlayerCharacter->GetPlayerItemUseComponent();
	if (ItemUseComponent)
	{
		return ItemUseComponent->UnUseItem(pSrc->ItemObject);
	}
	return false;
}

bool UInventoryComponent::HasEquipItem(UBaseItem* EquipItem) const
{
	if (!EquipItem)
		return false;
	if (ABasePlayer* Player = Cast<ABasePlayer>(GetOwner()))
	{
		if (UPlayerEquipComponent* EquipmentComponent = Cast< UPlayerEquipComponent>(Player->GetPlayerEquipComponent()))
		{
			return EquipmentComponent->IsEquipSlot();
		}
	}
	return false;
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

	if (Dst->ItemObject)
	{
		UItemDataAsset* DstDataAsset = Dst->ItemObject->GetPalItemDataAssetByName();
		if (DstDataAsset)
		{
			UItemDataSlotFragment* DstSlotFragment = Cast< UItemDataSlotFragment>(DstDataAsset->GetItemDataFragmentOfClass(UItemDataSlotFragment::StaticClass()));
			if (DstSlotFragment)
			{
				DstSlotType = DstSlotFragment->GetSlotType();
			}
		}
	}

	if (Src->SlotType == EItemSlotType::None)
	{
		UBaseItem* TempSrcItem = Src->ItemObject;
		Src->ItemObject = Dst->ItemObject;
		Dst->ItemObject = TempSrcItem;
		if (onUpdateInventory.IsBound())
		{
			onUpdateInventory.Broadcast();
		}
		return true;
	}
	//else  if (DstSlotType == Src->SlotType)
	//{
	//	bool bUseItemSlot = UseItemSlot(Dst);
	//	if (bUseItemSlot)
	//	{
	//		return RemoveItem(Dst, 1);
	//	}
	//}
	return false;
}

const FInventorySlot* UInventoryComponent::GetEquipSlot(EItemSlotType SlotType)
{
	if (EquipSlot.IsValidIndex(static_cast<uint8>(SlotType)))
	{
		return &EquipSlot[static_cast<uint8>(SlotType)];
	}
	return nullptr;
}

void UInventoryComponent::SetEquipSlot(EItemSlotType SlotType, UBaseItem* Item)
{
	EquipSlot[static_cast<uint8>(SlotType)].ItemObject = Item;
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