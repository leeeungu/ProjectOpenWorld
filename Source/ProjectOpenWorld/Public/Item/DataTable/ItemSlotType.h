#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemSlotType.generated.h"



UENUM()
enum class EItemSlotType : uint8
{
	None,
	Weapon,
	Axe,
	PickAxe,
	HeadEquip,
	BodyEquip,
	Shiled,
	Glider,
	SlotTypeEnumMax UMETA(Hidden)
};