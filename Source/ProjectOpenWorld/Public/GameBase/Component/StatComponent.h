#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Pal/Data/StatusData.h"
#include "StatComponent.generated.h"

struct FPalDamagePayload;
struct FPalMonsterLevelData;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnStatChangedCom, EStatusType, StatType, double, PreCurrentStat, double, CurrentStat);


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
#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif
protected:
	UPROPERTY(BlueprintAssignable)
	FOnStatChangedCom OnStatCurrentChanged{};
public:	
	FOnStatChanged* GetCurrentOnStatChanged(EStatusType StatName);
	FOnStatChanged* GetMaxOnStatChanged(EStatusType StatName);

	void StatBeginPlay(EStatusType StatName );
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
	void OnReceiveDamage(const FPalDamagePayload& DamagePayload);

	void SetPalLevelData(const FPalMonsterLevelData& LevelData);
};
