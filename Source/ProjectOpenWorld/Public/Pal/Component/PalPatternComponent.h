#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PalPatternComponent.generated.h"

class UPatternObj;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTOPENWORLD_API UPalPatternComponent : public UActorComponent
{
	GENERATED_BODY()
protected:

	UPROPERTY(VisibleAnywhere, Category = "PalPattern")
	TSet<TObjectPtr<UPatternObj>> PatternSet{};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "PalPattern", Instanced)
	TArray<TObjectPtr<UPatternObj>> PatternClassArray{};


public:
	UPalPatternComponent();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UFUNCTION(BlueprintCallable, Category = "PalPattern")
	void StartPattern(uint8 PatternIndex);
	UFUNCTION(BlueprintCallable, Category = "PalPattern")
	void EndPattern(uint8 PatternIndex);
	UFUNCTION(BlueprintCallable, Category = "PalPattern")
	void UpdatePatternCondition(uint8 PatternIndex);
	UFUNCTION(BlueprintPure, Category = "PalPattern")
	bool IsPatternActive(uint8 PatternIndex) const;


	//UFUNCTION(BlueprintCallable, Category = "PalPattern")
	//EPalPatternState GetCurrentPatternState() const;
	
};
