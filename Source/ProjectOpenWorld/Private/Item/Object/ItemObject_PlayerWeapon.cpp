#include "Item/Object/ItemObject_PlayerWeapon.h"
#include "Player/Character/BasePlayer.h"
#include "Components/SkeletalMeshComponent.h"
#include "Item/DataTable/WeaponeData.h"

UItemObject_PlayerWeapon::UItemObject_PlayerWeapon() :Super()
{
	EquipPlayerState = EPlayerState::Battle;
}
