#include "Player/Component/PlayerEquipComponent.h"
#include "Item/DataTable/WeaponeData.h"
#include "Player/Character/BasePlayer.h"
#include "Inventory/Component/InventoryComponent.h"
#include "Item/AssetUserData/WeaponeAssetUserData.h"
#include "Item/DataAsset/ItemDataAsset.h"
#include "Item/Object/BaseItem.h"
#include "Item/Object/Fragment/PlayerAnimationDataFragment.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Player/Animation/PlayerAnimInstance.h"
#include "Item/Object/Fragment/ItemDataSlotFragment.h"


UPlayerEquipComponent::UPlayerEquipComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerEquipComponent::BeginPlay()
{
	Super::BeginPlay();

	ABasePlayer* Player = Cast<ABasePlayer>(GetOwner());
	if (!Player)
		return;

	PlayerMesh = Player->GetMesh();
	if (!PlayerMesh)
		return;

	if (!WeaponMesh)
	{
		WeaponMesh = Player->GetRightHandEquipComponent();
		//Cast<USkeletalMeshComponent>(
		//	Player->AddComponentByClass(USkeletalMeshComponent::StaticClass(), false, FTransform::Identity, false));
	}

	if (!WeaponMesh)
		return;

	WeaponMesh->AttachToComponent(PlayerMesh, FAttachmentTransformRules::KeepRelativeTransform);
	WeaponMesh->SetRelativeTransform(FTransform::Identity);
	WeaponMesh->SetSkeletalMesh(nullptr);
}

bool UPlayerEquipComponent::EquipItem(const UBaseItem* Item)
{
	if (!Item || !WeaponMesh || !PlayerMesh)
		return false;

	if (CurrentEquipItem == Item)
		return true;
	ABasePlayer* Player = Cast<ABasePlayer>(GetOwner());
	UItemDataAsset* Asset = Item->GetPalItemDataAssetByName();
	if (!Player || !Asset)
		return false;
	const TArray<TObjectPtr<UItemDataFragment>>* Array = Asset->GetItemDataFragmentArray();
	FItemEquipContext Context{};
	Context.User = Player;
	Context.Target = WeaponMesh;
	Context.Item = Item;
	for (auto& Fragment : *Array)
	{
		if (IEquipItemInterface* UseInterface = Cast<IEquipItemInterface>(Fragment))
		{
			UseInterface->Equip(Context);
		}
	}

	UItemDataSlotFragment* SlotFragment = Cast< UItemDataSlotFragment>(Item->GetItemDataFragment(UItemDataSlotFragment::StaticClass()));
	if (SlotFragment)
	{
		const EWeapone NewWeaponType = SlotFragment->GetWeaponeData();

		// 휠 전환용 등록 정보는 유지되어야 하므로 Equip 시 갱신만 한다.
		EquipItemMap.FindOrAdd(NewWeaponType) = Item;
		CurrentEquipItem = Item;
		CurrentWeapone = NewWeaponType;
		if(OnEquipChanged.IsBound())
			OnEquipChanged.Broadcast(CurrentWeapone, CurrentEquipItem);

		Player->ChangePlayerState(SlotFragment->GetEquipPlayerState());
		Player->ChangeEquipWidget(SlotFragment->GetWeaponeID(), NewWeaponType);
	}
	return true;
}

bool UPlayerEquipComponent::UnequipItem(const UBaseItem* Item)
{
	if (!Item || !WeaponMesh)
		return false;

	// 현재 손에 들고 있는 아이템만 해제 가능
	if (CurrentEquipItem != Item)
		return false;

	ABasePlayer* Player = Cast<ABasePlayer>(GetOwner());
	if (!Player)
		return false;

	USkeletalMesh* CurrentMesh = WeaponMesh->GetSkeletalMeshAsset();
	UItemDataSlotFragment* SlotFragment = Cast< UItemDataSlotFragment>(Item->GetItemDataFragment(UItemDataSlotFragment::StaticClass()));
	if (SlotFragment)
	{
		Player->ChangePlayerState(SlotFragment->GetUnEquipPlayerState());
		Player->ChangeEquipWidget(NAME_None, EWeapone::None);
	}

	WeaponMesh->SetSkeletalMesh(nullptr);
	WeaponMesh->SetRelativeTransform(FTransform::Identity);

	CurrentEquipItem = nullptr;
	CurrentWeapone = EWeapone::None;
	if (OnEquipChanged.IsBound())
		OnEquipChanged.Broadcast(CurrentWeapone, CurrentEquipItem);
	if (UPlayerAnimInstance* PlayerAnimInstance = Player->GetPlayerAnimInstance())
	{
		PlayerAnimInstance->ResetAnimSection();
	}

	return true;
}

bool UPlayerEquipComponent::RegisterItem(const UBaseItem* Item)
{
	ABasePlayer* Player = Cast<ABasePlayer>(GetOwner());
	if (!Item || !WeaponMesh || !PlayerMesh || !Player)
		return false;

	UHandEquipItemFragment* HandEquipData = GetHandEquipFragment(Item);
	UItemDataSlotFragment* SlotFragment = Cast< UItemDataSlotFragment>(Item->GetItemDataFragment(UItemDataSlotFragment::StaticClass()));
	if (HandEquipData)
	{
		const EWeapone NewWeaponType = SlotFragment->GetWeaponeData();

		const UBaseItem*& RegisterItem = EquipItemMap.FindOrAdd(NewWeaponType, nullptr);
		if (RegisterItem && Player->GetInventoryComponent() && SlotFragment)
		{
			FInventorySlot* EmpthySlot = Player->GetInventoryComponent()->GetEmptyInventorySlot();
			const FInventorySlot* EquipSlot = Player->GetInventoryComponent()->GetEquipSlot(SlotFragment->GetSlotType());
			Player->GetInventoryComponent()->SwapEquipSlot(EquipSlot, EmpthySlot);
			//UE_LOG(LogTemp, Warning, TEXT("RegisterItem: Swapped item from equip slot to inventory slot"));
		}
		if (RegisterItem)
		{
			//UE_LOG(LogTemp, Warning, TEXT("PreRegistered item: %s to weapon type: %d"), *RegisterItem->GetItemID().ToString(), static_cast<uint8>(NewWeaponType));
		}
		RegisterItem = Item;
		//UE_LOG(LogTemp, Warning, TEXT("Registered item: %s to weapon type: %d"), *RegisterItem->GetItemID().ToString(), static_cast<uint8>(NewWeaponType));
		if (Player->GetInventoryComponent() && SlotFragment)
		{
			Player->GetInventoryComponent()->SetEquipSlot(SlotFragment->GetSlotType(), const_cast<UBaseItem*>(Item));
		}
	}
	return true;
}

bool UPlayerEquipComponent::UnRegisterItem(const UBaseItem* Item)
{
	if (!Item)
		return false;
	UnequipItem(Item);
	UHandEquipItemFragment* HandEquipData = GetHandEquipFragment(Item);
	UItemDataSlotFragment* SlotFragment = Cast< UItemDataSlotFragment>(Item->GetItemDataFragment(UItemDataSlotFragment::StaticClass()));
	if (HandEquipData && SlotFragment)
	{
		const EWeapone NewWeaponType = SlotFragment->GetWeaponeData();
		const UBaseItem*& RegisterItem = EquipItemMap.FindOrAdd(NewWeaponType, nullptr);
		if (RegisterItem)
		{
			RegisterItem = nullptr;
		}
		return true;
	}
	return false;
}

bool UPlayerEquipComponent::IsEquipSlot(const UBaseItem* Item) const
{
	if (!Item)
		return false;
	if (UItemDataSlotFragment* SlotFragment = Cast< UItemDataSlotFragment>(Item->GetItemDataFragment(UItemDataSlotFragment::StaticClass())))
	{
			return EquipItemMap.Find(SlotFragment->GetWeaponeData()) != nullptr;
	}
	return false;
}

bool UPlayerEquipComponent::UnEquipCurrent()
{
	return UnequipItem(CurrentEquipItem);
}

bool UPlayerEquipComponent::IsEquip() const
{
	return CurrentEquipItem != nullptr;
}

void UPlayerEquipComponent::StartEvent(const FInputActionValue& Value, EInputKeyType KeyType)
{
}

void UPlayerEquipComponent::TriggerEvent(const FInputActionValue& Value, EInputKeyType KeyType)
{
	const FVector2D AxisValue = Value.Get<FVector2D>();

	if (FMath::IsNearlyZero(AxisValue.X))
		return;

	const int32 Direction = AxisValue.X > 0.f ? 1 : -1;

	const int32 MinWeaponIndex = static_cast<int32>(EWeapone::None) + 1;
	const int32 MaxWeaponIndex = static_cast<int32>(EWeapone::WeaponeMax) - 1;

	int32 NextWeaponIndex = static_cast<int32>(CurrentWeapone);
	if (NextWeaponIndex < MinWeaponIndex || NextWeaponIndex > MaxWeaponIndex)
	{
		NextWeaponIndex = MinWeaponIndex;
	}
	else
	{
		NextWeaponIndex += Direction;

		if (NextWeaponIndex > MaxWeaponIndex)
			NextWeaponIndex = MinWeaponIndex;
		else if (NextWeaponIndex < MinWeaponIndex)
			NextWeaponIndex = MaxWeaponIndex;
	}

	if (CurrentEquipItem)
	{
		UnequipItem(CurrentEquipItem);
	}

	CurrentWeapone = static_cast<EWeapone>(NextWeaponIndex);

	if (const UBaseItem** FoundItem = EquipItemMap.Find(CurrentWeapone))
	{
		if (*FoundItem)
		{
			EquipItem(*FoundItem);
		}
	}
}

void UPlayerEquipComponent::CompleteEvent(const FInputActionValue& Value, EInputKeyType KeyType)
{
}

void UPlayerEquipComponent::OnUpdateEquip(const TArray<FInventorySlot>& Data)
{
	ABasePlayer* Player = Cast<ABasePlayer>(GetOwner());
	if (Player)
	{
		for (const FInventorySlot& Slot : Data)
		{
			if (const UBaseItem* Item = Slot.ItemObject)
			{
				UHandEquipItemFragment* HandEquipData = GetHandEquipFragment(Item);
				UItemDataSlotFragment* SlotFragment = Cast< UItemDataSlotFragment>(Item->GetItemDataFragment(UItemDataSlotFragment::StaticClass()));
				if (HandEquipData && SlotFragment)
				{
					const EWeapone NewWeaponType = SlotFragment->GetWeaponeData();
					const UBaseItem*& RegisterItem = EquipItemMap.FindOrAdd(NewWeaponType, nullptr);
					RegisterItem = Item;
				}

			}
		}
	}
}

UHandEquipItemFragment* UPlayerEquipComponent::GetHandEquipFragment(const UBaseItem* Item) const
{
	if (!Item || !UItemDataSubsystem::IsValidInstance())
		return nullptr;

	UItemDataAsset* ItemDataAsset = UItemDataSubsystem::GetPalItemDataAssetByName(Item->GetItemID());
	if (!ItemDataAsset)
		return nullptr;

	TObjectPtr < UItemDataFragment> Fragments = ItemDataAsset->GetItemDataFragmentOfClass(UHandEquipItemFragment::StaticClass());

	return Cast<UHandEquipItemFragment>(Fragments);
}

UPlayerAnimationSLEDataFragment* UPlayerEquipComponent::GetPlayerAnimationSLEDataFragment(const UBaseItem* Item) const
{
	if (!Item || !UItemDataSubsystem::IsValidInstance())
		return nullptr;

	UItemDataAsset* ItemDataAsset = UItemDataSubsystem::GetPalItemDataAssetByName(Item->GetItemID());
	if (!ItemDataAsset)
		return nullptr;

	TObjectPtr < UItemDataFragment> Fragments = ItemDataAsset->GetItemDataFragmentOfClass(UPlayerAnimationSLEDataFragment::StaticClass());

	return Cast<UPlayerAnimationSLEDataFragment>(Fragments);
}

UWeaponeAssetUserData* UPlayerEquipComponent::GetWeaponAssetUserData(USkeletalMesh* Mesh) const
{
	if (!Mesh)
		return nullptr;

	return Cast<UWeaponeAssetUserData>(Mesh->GetAssetUserDataOfClass(UWeaponeAssetUserData::StaticClass()));
}
