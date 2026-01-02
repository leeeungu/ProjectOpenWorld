#pragma once

#include "CoreMinimal.h"
#include "GameBase/MetaData/AMDLoop.h"
#include "AMDLoop_Count.generated.h"

class UAnimInstance;

UCLASS()
class PROJECTOPENWORLD_API UAnimLoopObject_Count : public UAnimLoopObject
{
	GENERATED_BODY()
private:
	int LoopCount{};
public:
	virtual void Initialize(UAnimInstance* OwnerAniminstance, UAMDLoop* MetaData) override;
	virtual void UpdateBlendOut() override;
};


UCLASS()
class PROJECTOPENWORLD_API UAMDLoop_Count : public UAMDLoop
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditInstanceOnly, Category = "Loop Data")
	uint8 LoopCount = 1;
public:
	UAMDLoop_Count (const FObjectInitializer& ObjectInitializer);
	UFUNCTION(BlueprintPure, Category = "Loop Data")
	uint8 GetLoopCount() const { return LoopCount; }
};
