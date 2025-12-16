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
	Max_PalCommandKind UMETA(Hidden),
};

UENUM(BlueprintType)
enum class ESubMoveType : uint8
{
	None_MoveType,
	Actor,
	Location,
	Max_MoveType UMETA(Hidden),
};

UENUM(BlueprintType)
enum class ESubAttackType : uint8
{
	Default,
	Skill01,
	Skill02,
	Max_AttackType UMETA(Hidden),
};

UENUM(BlueprintType)
enum class ESubWorkType : uint8
{
	None_WorkType,
	Architecture,
	Mining,
	Transport,
	Max_WorkType UMETA(Hidden),
};

USTRUCT(BlueprintType)
struct FPalCommand
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(VisibleAnywhere)
	EPalCommandKind CommandKind{};
	UPROPERTY(VisibleAnywhere)
	AActor* pInstigatorActor{};
	UPROPERTY(VisibleAnywhere)
	AActor* pTarget{};
	UPROPERTY(VisibleAnywhere)
	FVector TargetLocation{};
	UPROPERTY(VisibleAnywhere)
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