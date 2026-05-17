#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatComponent.generated.h"

struct FPalDamagePayload;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStatChanged, double, PreCurrentStat, double, CurrentStat);

UENUM(BlueprintType)
enum class EStatusType :uint8
{
	None UMETA(DisplayName = "None"),
	HP UMETA(DisplayName = "HP"),
	MP UMETA(DisplayName = "MP"),
	Stamina UMETA(DisplayName = "Stamina"),
	Attack UMETA(DisplayName = "Attack"),
	Defense UMETA(DisplayName = "Defense"),
	Speed UMETA(DisplayName = "Speed"),
};

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

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTOPENWORLD_API UStatComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Stat")
	TMap<EStatusType, FStatusData> StatusMap{};

protected:
	virtual void BeginPlay() override;
public:	
	FOnStatChanged* GetCurrentOnStatChanged(EStatusType StatName);
	FOnStatChanged* GetMaxOnStatChanged(EStatusType StatName);

	////virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(BlueprintCallable, Category = "Stat")
	virtual void SetCurrentStat(double Value = 0.0, EStatusType StatName = EStatusType::None);
	UFUNCTION(BlueprintCallable, Category = "Stat")
	virtual void SetMaxStat(double Value = 0.0, EStatusType StatName = EStatusType::None);
	UFUNCTION(BlueprintCallable, Category = "Stat")
	virtual double AddCurrentStat(double Value = 0.0, EStatusType StatName = EStatusType::None);
	UFUNCTION(BlueprintCallable, Category = "Stat")
	virtual double AddMaxStat(double Value = 0.0, EStatusType StatName = EStatusType::None);
	UFUNCTION(BlueprintPure, Category = "Stat")
	virtual double GetCurrentStat(EStatusType StatName = EStatusType::None) const;
	UFUNCTION(BlueprintPure, Category = "Stat")
	virtual double GetMaxStat(EStatusType StatName = EStatusType::None) const;
	UFUNCTION(BlueprintPure, Category = "Stat")
	virtual double GetStatPercent(EStatusType StatName = EStatusType::None) const;

	UFUNCTION()
	void ReceiveDamage(const FPalDamagePayload& DamagePayload);
};
