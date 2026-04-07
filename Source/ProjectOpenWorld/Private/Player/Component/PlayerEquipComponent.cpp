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
	if (Player)
	{
		PlayerMesh = Player->GetMesh();
		WeaponMesh = Cast< USkeletalMeshComponent>(Player->AddComponentByClass(USkeletalMeshComponent::StaticClass(), false, FTransform::Identity, false));
		WeaponMesh->AttachToComponent(PlayerMesh, FAttachmentTransformRules::KeepRelativeTransform);
	}
	
}

bool UPlayerEquipComponent::EquipItem(UBaseItem* Item)
{
	if (WeaponMesh && Item)
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
				WeaponMesh->SetSkeletalMesh(WeaponeData->HandEquipMesh);
				WeaponMesh->AttachToComponent(PlayerMesh, FAttachmentTransformRules::KeepRelativeTransform, WeaponeData->HandEquipSocket);
				WeaponMesh->SetRelativeTransform(WeaponeData->HandEquipRelativeTransform);
				CurrentEquipItem = Item;
				//SetEquipMesh(WeaponeData->HandEquipMesh);
				return true;
			}
		}
	}
	return false;
}

bool UPlayerEquipComponent::UnequipItem(UBaseItem* Item)
{
	if (WeaponMesh && Item)
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
	return false;
}

void UPlayerEquipComponent::StartEvent(const FInputActionValue& Value, EInputKeyType KeyType)
{
}

void UPlayerEquipComponent::TriggerEvent(const FInputActionValue& Value, EInputKeyType KeyType)
{
	FVector2D AxisValue = Value.Get<FVector2D>();
	int value = AxisValue.X / FMath::Abs(AxisValue.X);
	UE_LOG(LogTemp, Warning, TEXT("Equip Triggered! %d"), value);
	uint8 WeaponeIndex = static_cast<uint8>(CurrentWeapone);
	WeaponeIndex += value;
	if(WeaponeIndex <= 0)
		WeaponeIndex = static_cast<uint8>(EWeapone::WeaponeMax) - 1;
	if(WeaponeIndex >= static_cast<uint8>(EWeapone::WeaponeMax))
		WeaponeIndex = static_cast<uint8>(EWeapone::None) + 1;
	if (CurrentEquipItem)
	{
		UnequipItem(CurrentEquipItem);
	}
	CurrentWeapone = static_cast<EWeapone>(WeaponeIndex);
	TObjectPtr<UBaseItem>  Item = EquipItemMap.Contains(static_cast<EWeapone>(WeaponeIndex)) ? EquipItemMap[static_cast<EWeapone>(WeaponeIndex)] : nullptr;
	if (Item)
	{
		EquipItem(Item);
	}
}

void UPlayerEquipComponent::CompleteEvent(const FInputActionValue& Value, EInputKeyType KeyType)
{
}

//void UPlayerEquipComponent::SetEquipMesh(USkeletalMesh* NewMesh)
//{
//	ABasePlayer* Player = Cast<ABasePlayer>(GetOwner());
//	if (USkeletalMesh* CurrentMesh = WeaponMesh->GetSkeletalMeshAsset())
//	{
//		UWeaponeAssetUserData* WeaponData = Cast< UWeaponeAssetUserData>(CurrentMesh->GetAssetUserDataOfClass(UWeaponeAssetUserData::StaticClass()));
//		if (WeaponData)
//		{
//			Player->ChangePlayerState(WeaponData->GetUnEquipPlayerState());
//		}
//	}
//	if (NewMesh && PlayerMesh)
//	{
//		UWeaponeAssetUserData* WeaponData = Cast< UWeaponeAssetUserData>(NewMesh->GetAssetUserDataOfClass(UWeaponeAssetUserData::StaticClass()));
//		if (WeaponData)
//		{
//			EWeapone WeaponeType = WeaponData->GetWeaponeData();
//			FName SocketName = WeaponData->GetSocketName();
//			//= EquipSocket.Contains(WeaponeType) ? FName(*EquipSocket[WeaponeType]) : NAME_None;
//			WeaponMesh->SetSkeletalMesh(NewMesh);
//			WeaponMesh->SetRelativeTransform(FTransform::Identity);
//			WeaponMesh->AttachToComponent(PlayerMesh, FAttachmentTransformRules::KeepRelativeTransform, SocketName);
//			EquipMesh.FindOrAdd(WeaponeType) = NewMesh;
//			Player->ChangePlayerState(WeaponData->GetEquipPlayerState());
//			Player->ChangeEquipWidget(WeaponData->GetWeaponeID(), WeaponeType);
//			CurrentWeapone = WeaponeType;
//		}
//	}
//}
//void UPlayerEquipComponent::SetUnequipMesh(USkeletalMesh* Oldsh)
//{
//	ABasePlayer* Player = Cast<ABasePlayer>(GetOwner());
//	if (USkeletalMesh* CurrentMesh = WeaponMesh->GetSkeletalMeshAsset())
//	{
//		UWeaponeAssetUserData* WeaponData = Cast< UWeaponeAssetUserData>(CurrentMesh->GetAssetUserDataOfClass(UWeaponeAssetUserData::StaticClass()));
//		EWeapone WeaponeType = WeaponData ? WeaponData->GetWeaponeData() : EWeapone::None;
//		if (WeaponData)
//		{
//			Player->ChangePlayerState(WeaponData->GetUnEquipPlayerState());
//			EquipMesh.FindOrAdd(WeaponeType) = nullptr;
//		}
//	}
//	uint8 WeaponeIndex = static_cast<uint8>(CurrentWeapone);
//	WeaponeIndex += 1;
//	if (WeaponeIndex >= static_cast<uint8>(EWeapone::WeaponeMax))
//		WeaponeIndex = static_cast<uint8>(EWeapone::None) + 1;
//	TObjectPtr<USkeletalMesh>  Mesh = EquipMesh.Contains(static_cast<EWeapone>(WeaponeIndex)) ? EquipMesh[static_cast<EWeapone>(WeaponeIndex)] : nullptr;
//	if (Mesh)
//	{
//		SetEquipMesh(Mesh);
//	}
//}
//
//void UPlayerEquipComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}

