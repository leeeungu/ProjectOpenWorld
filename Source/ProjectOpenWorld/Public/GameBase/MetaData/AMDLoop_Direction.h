#pragma once

#include "CoreMinimal.h"
#include "GameBase/MetaData/AMDLoop.h"
#include "AMDLoop_Direction.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UAnimLoopObject_Direction : public UAnimLoopObject
{
	GENERATED_BODY()
private:
	FVector MoveWorldDirection{};
	float MoveSpeed = 3500.f;
	float MoveDistance = 3500.f;
	float CurrenDistance{};
	EMovementMode InitMovement{};
	uint8 InitCustomMovement{};

public:
	virtual void Initialize(UAnimInstance* Animinstance, UAMDLoop* MetaData) override;
	virtual void UpdateLoop(float DeltaTime = 0) override;
	virtual void EndLoop() override;
};


UCLASS()
class PROJECTOPENWORLD_API UAMDLoop_Direction : public UAMDLoop
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditInstanceOnly, Category = "Loop Data")
	FVector MoveWorldDirection{};
	UPROPERTY(EditInstanceOnly, Category = "Loop Data")
	float MoveSpeed = 3500.f;
	UPROPERTY(EditInstanceOnly, Category = "Loop Data")
	float MoveDistance= 3500.f;
	UPROPERTY(EditInstanceOnly, Category = "Loop Data")
	TEnumAsByte<EMovementMode> NewMovemntMode = EMovementMode::MOVE_Flying;
	UPROPERTY(EditInstanceOnly, Category = "Loop Data")
	uint8 CustomMovemntMode{};

public:
	UAMDLoop_Direction(const FObjectInitializer& ObjectInitializer);
	UFUNCTION(BlueprintPure, Category = "Loop Data")
	FVector GetMoveDirection() const { return MoveWorldDirection; }
	UFUNCTION(BlueprintPure, Category = "Loop Data")
	float GetMoveSpeed() const { return  MoveSpeed; }
	UFUNCTION(BlueprintPure, Category = "Loop Data")
	float GetMoveDistance() const { return MoveDistance; }
	UFUNCTION(BlueprintPure, Category = "Loop Data")
	EMovementMode GetMovemntMode() const { return NewMovemntMode; }
	UFUNCTION(BlueprintPure, Category = "Loop Data")
	uint8 GetCustomMovemntMode() const { return CustomMovemntMode; }
};
