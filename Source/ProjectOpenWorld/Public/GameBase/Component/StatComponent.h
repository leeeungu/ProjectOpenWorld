#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTOPENWORLD_API UStatComponent : public UActorComponent
{
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere, Category = "Stat")
	double CurrentStat{};
	UPROPERTY(EditAnywhere, Category = "Stat")
	double MaxStat = std::numeric_limits<double>::max();

public:	
	UStatComponent();

protected:
	virtual void BeginPlay() override;

public:	
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(BlueprintCallable, Category = "Stat")
	virtual void SetCurrentStat(double Value); 
	UFUNCTION(BlueprintCallable, Category = "Stat")
	virtual void SetMaxStat(double Value);
	UFUNCTION(BlueprintCallable, Category = "Stat")
	virtual double AddCurrentStat(double Value);
	UFUNCTION(BlueprintCallable, Category = "Stat")
	virtual double AddMaxStat(double Value);
	UFUNCTION(BlueprintPure, Category = "Stat")
	virtual double GetCurrentStat() const { return CurrentStat; }
	UFUNCTION(BlueprintPure, Category = "Stat")
	virtual double GetMaxStat() const { return MaxStat; }
	UFUNCTION(BlueprintPure, Category = "Stat")
	virtual double GetStatPercent() const;
};
