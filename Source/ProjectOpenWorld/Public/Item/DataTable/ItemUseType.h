#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemUseType.generated.h"

UENUM(BlueprintType)
enum class EItemUseType : uint8
{
	None		UMETA(DisplayName = "None"),
	HandEquip	UMETA(DisplayName = "Hand Equip"),
	HeadEquip	UMETA(DisplayName = "Head Equip"),
	BodyEquip	UMETA(DisplayName = "Body Equip"),
	Consume		UMETA(DisplayName = "Consume"),
	Throw		UMETA(DisplayName = "Throw"),
	UseTypeEnumMax UMETA(Hidden)
};
