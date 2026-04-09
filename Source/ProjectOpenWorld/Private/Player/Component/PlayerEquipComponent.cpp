#include "Player/Component/PlayerEquipComponent.h"
#include "Item/DataTable/WeaponeData.h"
#include "Player/Character/BasePlayer.h"
#include "Item/AssetUserData/WeaponeAssetUserData.h"
#include "Item/DataAsset/ItemDataAsset.h"
#include "Item/Object/BaseItem.h"
#include "Item/System/ItemDataSubsystem.h"

UPlayerEquipComponent::UPlayerEquipComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	//EquipSocket.Add(EWeapone::Sword, TEXT("WeaponR_Sword"));
	//EquipSocket.Add(EWeapone::Bow, TEXT("WeaponR_Bow"));
	//EquipSocket.Add(EWeapone::Spear, TEXT("WeaponR_Spear"));
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
		WeaponMesh = Cast<USkeletalMeshComponent>(
			Player->AddComponentByClass(USkeletalMeshComponent::StaticClass(), false, FTransform::Identity, false));
	}

	if (!WeaponMesh)
		return;

	WeaponMesh->AttachToComponent(PlayerMesh, FAttachmentTransformRules::KeepRelativeTransform);
	WeaponMesh->SetRelativeTransform(FTransform::Identity);
	WeaponMesh->SetSkeletalMesh(nullptr);
}

bool UPlayerEquipComponent::EquipItem(UBaseItem* Item)
{
	//if (WeaponMesh && Item)
	//{
	//	FName ItemID = Item->GetItemID();
	//	UItemDataAsset* ItemDataAsset =	UItemDataSubsystem::GetPalItemDataAssetByName(ItemID);
	//	const TArray<TObjectPtr<UItemDataFragment>> HandEquipFragments =
	//	ItemDataAsset->GetItemDataFragmentOfClass(UHandEquipItemFragment::StaticClass());

	//	if (HandEquipFragments.Num() > 0)
	//	{
	//		UHandEquipItemFragment* WeaponeData = Cast<UHandEquipItemFragment>(HandEquipFragments[0]);
	//		if (WeaponeData)
	//		{
	//			WeaponMesh->SetSkeletalMesh(WeaponeData->HandEquipMesh);
	//			WeaponMesh->AttachToComponent(PlayerMesh, FAttachmentTransformRules::KeepRelativeTransform, WeaponeData->HandEquipSocket);
	//			WeaponMesh->SetRelativeTransform(WeaponeData->HandEquipRelativeTransform);
	//			CurrentEquipItem = Item;
	//			//SetEquipMesh(WeaponeData->HandEquipMesh);
	//			return true;
	//		}
	//	}
	//}
	//return false;

	if (!Item || !WeaponMesh || !PlayerMesh)
		return false;

	if (CurrentEquipItem == Item)
		return true;

	UHandEquipItemFragment* HandEquipData = GetHandEquipFragment(Item);
	if (!HandEquipData || !HandEquipData->HandEquipMesh)
		return false;

	UWeaponeAssetUserData* NewWeaponData = GetWeaponAssetUserData(HandEquipData->HandEquipMesh);
	if (!NewWeaponData)
		return false;

	if (CurrentEquipItem && CurrentEquipItem != Item)
	{
		if (!UnequipItem(CurrentEquipItem))
			return false;
	}
	ABasePlayer* Player = Cast<ABasePlayer>(GetOwner());

	WeaponMesh->SetSkeletalMesh(HandEquipData->HandEquipMesh);
	WeaponMesh->AttachToComponent(
		PlayerMesh,
		FAttachmentTransformRules::KeepRelativeTransform,
		HandEquipData->HandEquipSocket);
	WeaponMesh->SetRelativeTransform(HandEquipData->HandEquipRelativeTransform);

	const EWeapone NewWeaponType = NewWeaponData->GetWeaponeData();

	// 휠 전환용 등록 정보는 유지되어야 하므로 Equip 시 갱신만 한다.
	EquipItemMap.FindOrAdd(NewWeaponType) = Item;

	CurrentEquipItem = Item;
	CurrentWeapone = NewWeaponType;

	if (Player)
	{
		Player->ChangePlayerState(NewWeaponData->GetEquipPlayerState());
		Player->ChangeEquipWidget(NewWeaponData->GetWeaponeID(), NewWeaponType);
	}

	return true;
}

bool UPlayerEquipComponent::UnequipItem(UBaseItem* Item)
{
	/*if (WeaponMesh && Item)
	{
		FName ItemID = Item->GetItemID();
		UItemDataAsset* ItemDataAsset =	UItemDataSubsystem::GetPalItemDataAssetByName(ItemID);
		const TArray<TObjectPtr<UItemDataFragment>> HandEquipFragments =
		ItemDataAsset->GetItemDataFragmentOfClass(UHandEquipItemFragment::StaticClass());
		if (HandEquipFragments.Num() > 0)
		{
			UHandEquipItemFragment* WeaponeData = Cast<UHandEquipItemFragment>(HandEquipFragments[0]);
			if (WeaponeData)
			{
				WeaponMesh->SetSkeletalMesh(nullptr);
				CurrentEquipItem = nullptr;
				return true;
			}
		}
	}
	return false;*/

	if (!Item || !WeaponMesh)
		return false;

	// 현재 손에 들고 있는 아이템만 해제 가능
	if (CurrentEquipItem != Item)
		return false;

	ABasePlayer* Player = Cast<ABasePlayer>(GetOwner());
	USkeletalMesh* CurrentMesh = WeaponMesh->GetSkeletalMeshAsset();
	UWeaponeAssetUserData* CurrentWeaponData = GetWeaponAssetUserData(CurrentMesh);

	if (Player && CurrentWeaponData)
	{
		Player->ChangePlayerState(CurrentWeaponData->GetUnEquipPlayerState());
		Player->ChangeEquipWidget(NAME_None, EWeapone::None);
	}

	WeaponMesh->SetSkeletalMesh(nullptr);
	WeaponMesh->SetRelativeTransform(FTransform::Identity);

	CurrentEquipItem = nullptr;
	CurrentWeapone = EWeapone::None;

	// 주의:
	// EquipItemMap은 비우지 않는다.
	// 이 맵은 "현재 손에 들고 있는 것"이 아니라 "무기 타입별 등록 아이템"이어야
	// 휠 전환 시 다시 꺼내 쓸 수 있다.

	return true;
}

void UPlayerEquipComponent::StartEvent(const FInputActionValue& Value, EInputKeyType KeyType)
{
}

void UPlayerEquipComponent::TriggerEvent(const FInputActionValue& Value, EInputKeyType KeyType)
{
	//FVector2D AxisValue = Value.Get<FVector2D>();
	//if (FMath::IsNearlyZero(AxisValue.X))
	//	return;
	//int value = AxisValue.X / FMath::Abs(AxisValue.X);
	//UE_LOG(LogTemp, Warning, TEXT("Equip Triggered! %d"), value);
	//uint8 WeaponeIndex = static_cast<uint8>(CurrentWeapone);
	//WeaponeIndex += value;
	//if(WeaponeIndex <= 0)
	//	WeaponeIndex = static_cast<uint8>(EWeapone::WeaponeMax) - 1;
	//if(WeaponeIndex >= static_cast<uint8>(EWeapone::WeaponeMax))
	//	WeaponeIndex = static_cast<uint8>(EWeapone::None) + 1;
	//if (CurrentEquipItem)
	//{
	//	UnequipItem(CurrentEquipItem);
	//}
	//CurrentWeapone = static_cast<EWeapone>(WeaponeIndex);
	//TObjectPtr<UBaseItem>  Item = EquipItemMap.Contains(static_cast<EWeapone>(WeaponeIndex)) ? EquipItemMap[static_cast<EWeapone>(WeaponeIndex)] : nullptr;
	//if (Item)
	//{
	//	EquipItem(Item);
	//}
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

	if (TObjectPtr<UBaseItem>* FoundItem = EquipItemMap.Find(CurrentWeapone))
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

UHandEquipItemFragment* UPlayerEquipComponent::GetHandEquipFragment(UBaseItem* Item)
{
	if (!Item || !UItemDataSubsystem::IsValidInstance())
		return nullptr;

	UItemDataAsset* ItemDataAsset = UItemDataSubsystem::GetPalItemDataAssetByName(Item->GetItemID());
	if (!ItemDataAsset)
		return nullptr;

	const TArray<TObjectPtr<UItemDataFragment>> Fragments =
		ItemDataAsset->GetItemDataFragmentOfClass(UHandEquipItemFragment::StaticClass());

	if (Fragments.IsEmpty())
		return nullptr;

	return Cast<UHandEquipItemFragment>(Fragments[0]);
}

UWeaponeAssetUserData* UPlayerEquipComponent::GetWeaponAssetUserData(USkeletalMesh* Mesh)
{
	if (!Mesh)
		return nullptr;

	return Cast<UWeaponeAssetUserData>(
		Mesh->GetAssetUserDataOfClass(UWeaponeAssetUserData::StaticClass()));
}
