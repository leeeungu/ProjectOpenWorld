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


UENUM(BlueprintType, Category = "Weapon")
enum class EWeapone : uint8
{
	None,
	Hand,
	StoneSpear,
	Bow,
	Sword,
	PixAxe,
	Axe,
	PalSphere,
	WeaponeMax UMETA(Hidden)
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

USTRUCT(BlueprintType)
struct FWeaponUIData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	EWeapone WeaponeType{ EWeapone::None };
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UTexture2D> WeaponeImage{ nullptr };	
};