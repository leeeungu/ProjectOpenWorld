#pragma once

#include "CoreMinimal.h"
#include "GameBase/MetaData/AMDLoop.h"
#include "AMDLoop_Location.generated.h"

class UCurveVector;

class UAnimInstance;

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
	virtual void EndLoop() override;
};


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
	UPROPERTY(EditInstanceOnly, Category = "Loop Data")
	TEnumAsByte<EMovementMode> NewMovemntMode = EMovementMode::MOVE_Flying;
	UPROPERTY(EditInstanceOnly, Category = "Loop Data")
	uint8 CustomMovemntMode{};
public:
	UAMDLoop_Location(const FObjectInitializer& ObjectInitializer);
	UFUNCTION(BlueprintPure, Category = "Loop Data")
	FVector GetMoveLocation() const { return MoveLocation; }
	UFUNCTION(BlueprintPure, Category = "Loop Data")
	float GetMoveSpeed() const { return  MoveSpeed; }
	UFUNCTION(BlueprintPure, Category = "Loop Data")
	UCurveVector* GetSpeedData() const { return MoveOffsetData; }
	UFUNCTION(BlueprintPure, Category = "Loop Data")
	EMovementMode GetMovemntMode() const { return NewMovemntMode; }
	UFUNCTION(BlueprintPure, Category = "Loop Data")
	uint8 GetCustomMovemntMode() const { return CustomMovemntMode; }
};
