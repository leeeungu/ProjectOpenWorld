#pragma once

#include "CoreMinimal.h"
#include "PalJobTypes.generated.h"


class AActor;

UENUM(BlueprintType)
enum class EPalJobType : uint8
{
	None UMETA(DisplayName = "None"),
	Architecture UMETA(DisplayName = "Architecture"),
	Mining UMETA(DisplayName = "Mining"),
	Deforest UMETA(DisplayName = "Deforest"),
	Transport UMETA(DisplayName = "Transport"),
	Cooking UMETA(DisplayName = "Cooking"),
	Planting UMETA(DisplayName = "Planting"),
	Attack UMETA(DisplayName = "Attack"),
	CommonWork UMETA(DisplayName = "CommonWork"),
	Max_JobType UMETA(Hidden),
};

UENUM(meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EPalWorkCapability : uint8
{
	Architecture = 1 << 0,
	Mining = 1 << 1,
	Deforest = 1 << 2,
	Transport = 1 << 3,
	Cooking = 1 << 4,
	Planting = 1 << 5,
	Attack = 1 << 6,
	CommonWork = 1 <<7
};
ENUM_CLASS_FLAGS(EPalWorkCapability);

namespace PalJobUtils
{
	EPalWorkCapability ToCapability(EPalJobType Type);
}

USTRUCT(BlueprintType)
struct FPalWorkCommand
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(VisibleAnywhere)
	EPalJobType JobType{};
	UPROPERTY(VisibleAnywhere)
	TWeakObjectPtr<AActor> pInstigatorActor{};
	UPROPERTY(VisibleAnywhere)
	TWeakObjectPtr<AActor> pTarget{};
	UPROPERTY(VisibleAnywhere)
	FVector TargetLocation = FVector::ZeroVector;
	UPROPERTY(VisibleAnywhere)
	int Priority = 0;

	void Reset()
	{
		JobType = EPalJobType::None;
		pInstigatorActor.Reset();
		pTarget.Reset();
		TargetLocation = FVector::ZeroVector;
		Priority = 0;
	}
	virtual bool IsValidWork() const;
};
