#include "Player/Component/PlayerEquipComponent.h"
#include "Item/DataTable/WeaponeData.h"
#include "Player/Character/BasePlayer.h"
#include "Item/AssetUserData/WeaponeAssetUserData.h"
#include "Item/DataAsset/ItemDataAsset.h"
#include "Item/Object/BaseItem.h"
#include "Item/Object/Fragment/PlayerAnimationDataFragment.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Player/Animation/PlayerAnimInstance.h"

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
	ABasePlayer* Player = Cast<ABasePlayer>(GetOwner());
	if (!Player)
		return false;

	UHandEquipItemFragment* HandEquipData = GetHandEquipFragment(Item);
	if (HandEquipData && HandEquipData->HandEquipMesh)
	{
		UWeaponeAssetUserData* NewWeaponData = GetWeaponAssetUserData(HandEquipData->HandEquipMesh);
		if (NewWeaponData)
		{
			if (CurrentEquipItem && CurrentEquipItem != Item)
			{
				if (!UnequipItem(CurrentEquipItem))
					return false;
			}
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
			Player->ChangePlayerState(NewWeaponData->GetEquipPlayerState());
			Player->ChangeEquipWidget(NewWeaponData->GetWeaponeID(), NewWeaponType);
		}
	}

	TObjectPtr < UPlayerAnimationSLEDataFragment> Fragment = GetPlayerAnimationSLEDataFragment(Item);
	if (Fragment)
	{
		UE_LOG(LogTemp, Log, TEXT("SetAnimSequence"));
		if(UPlayerAnimInstance* PlayerAnimInstance = Player->GetPlayerAnimInstance())
		{
			PlayerAnimInstance->SetAnimationSequences(
				Fragment->GetStartAnim(),
				Fragment->GetLoopAnim(),
				Fragment->GetEndAnim());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No PlayerAnimationSLEDataFragment found for item: %s"), *Item->GetItemID().ToString());
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
	if (!Player)
		return false;

	USkeletalMesh* CurrentMesh = WeaponMesh->GetSkeletalMeshAsset();
	UWeaponeAssetUserData* CurrentWeaponData = GetWeaponAssetUserData(CurrentMesh);

	if (CurrentWeaponData)
	{
		Player->ChangePlayerState(CurrentWeaponData->GetUnEquipPlayerState());
		Player->ChangeEquipWidget(NAME_None, EWeapone::None);
	}

	WeaponMesh->SetSkeletalMesh(nullptr);
	WeaponMesh->SetRelativeTransform(FTransform::Identity);

	CurrentEquipItem = nullptr;
	CurrentWeapone = EWeapone::None;


	if (UPlayerAnimInstance* PlayerAnimInstance = Player->GetPlayerAnimInstance())
	{
		PlayerAnimInstance->ResetAnimSection();
	}

	return true;
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

	TObjectPtr < UItemDataFragment> Fragments = ItemDataAsset->GetItemDataFragmentOfClass(UHandEquipItemFragment::StaticClass());

	return Cast<UHandEquipItemFragment>(Fragments);
}

UPlayerAnimationSLEDataFragment* UPlayerEquipComponent::GetPlayerAnimationSLEDataFragment(UBaseItem* Item)
{
	if (!Item || !UItemDataSubsystem::IsValidInstance())
		return nullptr;

	UItemDataAsset* ItemDataAsset = UItemDataSubsystem::GetPalItemDataAssetByName(Item->GetItemID());
	if (!ItemDataAsset)
		return nullptr;

	TObjectPtr < UItemDataFragment> Fragments = ItemDataAsset->GetItemDataFragmentOfClass(UPlayerAnimationSLEDataFragment::StaticClass());

	return Cast<UPlayerAnimationSLEDataFragment>(Fragments);
}

UWeaponeAssetUserData* UPlayerEquipComponent::GetWeaponAssetUserData(USkeletalMesh* Mesh)
{
	if (!Mesh)
		return nullptr;

	return Cast<UWeaponeAssetUserData>(
		Mesh->GetAssetUserDataOfClass(UWeaponeAssetUserData::StaticClass()));
}
