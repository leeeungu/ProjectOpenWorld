#pragma once

#include "CoreMinimal.h"
#include "PalCommandData.generated.h"

UENUM(BlueprintType)
enum class EPalCommandKind : uint8
{
	None_PalCommandKind,
	Move,
	Attack,
	Interact,
	Work,
	Max_PalCommandKind,
};

UENUM(BlueprintType)
enum class ESubMoveType : uint8
{
	None_MoveType,
	Actor,
	Location,
	Max_MoveType,
};

UENUM(BlueprintType)
enum class ESubAttackType : uint8
{
	None_AttackType,
	AttackOnce,
	AttackLoop,
	Max_AttackType,
};

UENUM(BlueprintType)
enum class ESubWorkType : uint8
{
	None_WorkType,
	Architecture,
	Mining,
	Max_WorkType,
};

USTRUCT(BlueprintType)
struct FPalCommand
{
	GENERATED_USTRUCT_BODY()
public:
	EPalCommandKind CommandKind{};
	AActor* pInstigatorActor{};
	AActor* pTarget{};
	FVector TargetLocation{};
	uint8 SubCommandType{};

	FPalCommand& operator=(const FPalCommand& rhs)
	{
		CommandKind = rhs.CommandKind ;
		pInstigatorActor = rhs.pInstigatorActor ;
		pTarget = rhs.pTarget ;
		TargetLocation = rhs.TargetLocation ;
		SubCommandType = rhs.SubCommandType;
		return *this;
	}
	bool operator==(const FPalCommand& rhs)
	{
		if (CommandKind == rhs.CommandKind &&
			pInstigatorActor == rhs.pInstigatorActor &&
			pTarget == rhs.pTarget &&
			TargetLocation == rhs.TargetLocation &&
			SubCommandType == rhs.SubCommandType)
			return true;
		return false;
	}
};