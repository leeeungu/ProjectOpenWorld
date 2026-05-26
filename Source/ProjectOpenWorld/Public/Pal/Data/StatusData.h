#pragma once

#include "CoreMinimal.h"
#include "StatusData.generated.h"

enum class EPalJobType : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStatChanged, double, PreCurrentStat, double, CurrentStat);

UENUM(BlueprintType)
enum class EStatusType :uint8
{
	None UMETA(DisplayName = "None"),
	HP UMETA(DisplayName = "HP"),
	MP UMETA(DisplayName = "MP"),
	MoveSpeed UMETA(DisplayName = "MoveSpeed"),
	Stamina UMETA(DisplayName = "Stamina"),
	Attack UMETA(DisplayName = "Attack"),
	Defense UMETA(DisplayName = "Defense"),
	Speed UMETA(DisplayName = "Speed"),
	Architecture UMETA(DisplayName = "Architecture"),
	Mining UMETA(DisplayName = "Mining"),
	Deforest UMETA(DisplayName = "Deforest"),
	TransportSpeed UMETA(DisplayName = "TransportSpeed"),
};

namespace PalStatus
{
	EStatusType GetJobWorkSpeedStatus(EPalJobType JobType);
}

USTRUCT()
struct FStatusValue
{
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere, Category = "Stat")
	double Value = 0.0f;
public:
	UPROPERTY(BlueprintAssignable, Category = "Stat")
	FOnStatChanged OnChanged{};
	void SetValue(double NewValue);
	double GetValue() const { return Value; }
	double AddValue(double DeltaValue);
};

USTRUCT()
struct FStatusData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Stat")
	FStatusValue CurrentStatValue{};
	UPROPERTY(EditAnywhere, Category = "Stat")
	FStatusValue MaxStatValue{};

	void BeginPlay();
};