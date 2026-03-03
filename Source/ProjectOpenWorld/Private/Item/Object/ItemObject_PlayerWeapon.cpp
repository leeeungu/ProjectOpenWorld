#include "Item/Object/ItemObject_PlayerWeapon.h"
#include "Player/Character/BasePlayer.h"
#include "Components/SkeletalMeshComponent.h"

UItemObject_PlayerWeapon::UItemObject_PlayerWeapon() :Super()
{
	EquipPlayerState = EPlayerState::Battle;
}

void UItemObject_PlayerWeapon::RegisterItemObject(AActor* TargetActor)
{
	ABasePlayer* Player = Cast<ABasePlayer>(TargetActor);
	if (Player && WeaponMesh)
	{
		Player->SetWeaponMesh(WeaponMesh.Get(), WeaponSocketName);
		Player->ChangePlayerState(EquipPlayerState);
	}
}

void UItemObject_PlayerWeapon::UnregisterItemObject(AActor* TargetActor)
{
	ABasePlayer* Player = Cast<ABasePlayer>(TargetActor);
	if (Player)
	{
		Player->SetWeaponMesh(nullptr, WeaponSocketName);
		Player->ChangePlayerState(EPlayerState::Travel);
	}
}
