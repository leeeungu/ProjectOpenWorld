#include "Item/Object/ItemObject_Equip.h"
#include "Player/Character/BasePlayer.h"

void UItemObject_Equip::RegisterItemObject(AActor* TargetActor)
{
	ABasePlayer* Player = Cast<ABasePlayer>(TargetActor);
	if (Player && WeaponMesh)
	{
		Player->SetWeaponMesh(WeaponMesh.Get());
	}
}

void UItemObject_Equip::UnregisterItemObject(AActor* TargetActor)
{

	ABasePlayer* Player = Cast<ABasePlayer>(TargetActor);
	if (Player && WeaponMesh)
	{
		Player->UnEquip(WeaponMesh.Get());
	}
}
