#include "Player/Component/PlayerEquipComponent.h"
#include "Item/DataTable/WeaponeData.h"
#include "Player/Character/BasePlayer.h"
#include "Item/AssetUserData/WeaponeAssetUserData.h"

UPlayerEquipComponent::UPlayerEquipComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	EquipSocket.Add(EWeapone::Sword, TEXT("WeaponR_Sword"));
	EquipSocket.Add(EWeapone::Bow, TEXT("WeaponR_Bow"));
	//EquipSocket.Add(EWeapone::Spear, TEXT("WeaponR_Spear"));
}

void UPlayerEquipComponent::BeginPlay()
{
	Super::BeginPlay();
	ABasePlayer* Player = Cast<ABasePlayer>(GetOwner());
	if (Player)
	{
		PlayerMesh = Player->GetMesh();
	}
	
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
	//CurrentWeapone = static_cast<EWeapone>(WeaponeIndex);
	TObjectPtr<USkeletalMesh>  Mesh = EquipMesh.Contains(static_cast<EWeapone>(WeaponeIndex)) ? EquipMesh[static_cast<EWeapone>(WeaponeIndex)] : nullptr;
	if (Mesh)
	{
		SetEquipMesh(Mesh);
	}
}

void UPlayerEquipComponent::CompleteEvent(const FInputActionValue& Value, EInputKeyType KeyType)
{
}

void UPlayerEquipComponent::SetEquipMesh(USkeletalMesh* NewMesh)
{
	ABasePlayer* Player = Cast<ABasePlayer>(GetOwner());
	if (USkeletalMesh* CurrentMesh = GetSkeletalMeshAsset())
	{
		UWeaponeAssetUserData* WeaponData = Cast< UWeaponeAssetUserData>(CurrentMesh->GetAssetUserDataOfClass(UWeaponeAssetUserData::StaticClass()));
		if (WeaponData)
		{
			Player->ChangePlayerState(WeaponData->GetUnEquipPlayerState());
		}
	}
	if (NewMesh && PlayerMesh)
	{
		UWeaponeAssetUserData* WeaponData = Cast< UWeaponeAssetUserData>(NewMesh->GetAssetUserDataOfClass(UWeaponeAssetUserData::StaticClass()));
		if (WeaponData)
		{
			EWeapone WeaponeType = WeaponData->GetWeaponeData();
			FName SocketName = EquipSocket.Contains(WeaponeType) ? FName(*EquipSocket[WeaponeType]) : NAME_None;
			SetSkeletalMesh(NewMesh);
			SetRelativeTransform(FTransform::Identity);
			AttachToComponent(PlayerMesh, FAttachmentTransformRules::KeepRelativeTransform, SocketName);
			EquipMesh.FindOrAdd(WeaponeType) = NewMesh;
			Player->ChangePlayerState(WeaponData->GetEquipPlayerState());
			Player->ChangeEquipWidget(WeaponData->GetWeaponeID(), WeaponeType);
			CurrentWeapone = WeaponeType;
		}
	}
}
void UPlayerEquipComponent::SetUnequipMesh(USkeletalMesh* Oldsh)
{
	ABasePlayer* Player = Cast<ABasePlayer>(GetOwner());
	if (USkeletalMesh* CurrentMesh = GetSkeletalMeshAsset())
	{
		UWeaponeAssetUserData* WeaponData = Cast< UWeaponeAssetUserData>(CurrentMesh->GetAssetUserDataOfClass(UWeaponeAssetUserData::StaticClass()));
		EWeapone WeaponeType = WeaponData ? WeaponData->GetWeaponeData() : EWeapone::None;
		if (WeaponData)
		{
			Player->ChangePlayerState(WeaponData->GetUnEquipPlayerState());
			EquipMesh.FindOrAdd(WeaponeType) = nullptr;
		}
	}
	uint8 WeaponeIndex = static_cast<uint8>(CurrentWeapone);
	WeaponeIndex += 1;
	if (WeaponeIndex >= static_cast<uint8>(EWeapone::WeaponeMax))
		WeaponeIndex = static_cast<uint8>(EWeapone::None) + 1;
	TObjectPtr<USkeletalMesh>  Mesh = EquipMesh.Contains(static_cast<EWeapone>(WeaponeIndex)) ? EquipMesh[static_cast<EWeapone>(WeaponeIndex)] : nullptr;
	if (Mesh)
	{
		SetEquipMesh(Mesh);
	}
}
//
//void UPlayerEquipComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}

