#pragma once

#include "CoreMinimal.h"
#include "GameBase/MetaData/AMDLoop.h"
#include "AMDLoop_Location.generated.h"

class UCurveVector;

UCLASS()
class PROJECTOPENWORLD_API UAMDLoop_Location : public UAMDLoop
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditInstanceOnly, Category = "Loop Data")
	FVector MoveLocation{};
	UPROPERTY(EditInstanceOnly, Category = "Loop Data")
	float MoveSpeed = 3500.f;
	UPROPERTY(EditInstanceOnly, Category = "Loop Data")
	TObjectPtr < UCurveVector> MoveOffsetData{};
public:
	UAMDLoop_Location(const FObjectInitializer& ObjectInitializer);
	UFUNCTION(BlueprintPure, Category = "Loop Data")
	FVector GetMoveLocation() const { return MoveLocation; }
	UFUNCTION(BlueprintPure, Category = "Loop Data")
	float GetMoveSpeed() const { return  MoveSpeed; }
	UFUNCTION(BlueprintPure, Category = "Loop Data")
	UCurveVector* GetSpeedData() const { return MoveOffsetData; }
};

UCLASS()
class PROJECTOPENWORLD_API UAnimLoopObject_Location : public UAnimLoopObject
{
	GENERATED_BODY()
private:
	FVector MoveLocation{};
	FVector MoveDir{};
	FVector StartLocation{};
	float InitDisSq{};
	EMovementMode InitMovement{};
	uint8 InitCustomMovement{};
private:
	bool CheckDisSq();
public:
	virtual void Initialize(UAnimInstance* Animinstance, UAMDLoop* MetaData) override;
	virtual void UpdateLoop(float DeltaTime = 0) override;
};
