#pragma once

#include "CoreMinimal.h"
#include "WeaponeData.generated.h"


UENUM(BlueprintType, Category = "Weapon")
enum class EWeaponeType : uint8
{
	None,
	Melee,
	Ranged,
	Thrown,
};

UENUM(BlueprintType, Category = "Weapon")
enum class EWeaponeUseType : uint8
{
	None,
	Infinite,
	Consumable,
};


USTRUCT(BlueprintType)
struct FWeaponData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	EWeaponeType WeaponeType{ EWeaponeType::None };
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	EWeaponeUseType bIsInfinite{ false };
};