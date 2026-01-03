#pragma once

#include "CoreMinimal.h"
#include "GameBase/MetaData/AMDLoop.h"
#include "AMDLoop_Time.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UAMDLoop_Time : public UAMDLoop
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditInstanceOnly, Category = "Loop Data")
	float LoopTime = 1;
public:
	UAMDLoop_Time(const FObjectInitializer& ObjectInitializer);
	UFUNCTION(BlueprintPure, Category = "Loop Data")
	float GetLoopTime() const { return  LoopTime; }
};

UCLASS()
class PROJECTOPENWORLD_API UAnimLoopObject_Time : public UAnimLoopObject
{
	GENERATED_BODY()
private:
	float LoopTime{};
	float CurrentTime{};
private:
	bool CheckDisSq();
public:
	virtual void Initialize(UAnimInstance* Animinstance, UAMDLoop* MetaData) override;
	virtual void UpdateLoop(float DeltaTime = 0) override;
	virtual void EndLoop() override;
};

