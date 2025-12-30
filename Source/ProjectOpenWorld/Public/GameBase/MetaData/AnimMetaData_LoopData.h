#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimMetaData.h"
#include "GameBase/Interface/AnimMetaDataInterface.h"
#include "GameBase/Class/TimerClass.h"
#include "AnimMetaData_LoopData.generated.h"

class ACharacter;
class UCurveVector;

UCLASS()
class PROJECTOPENWORLD_API UAnimMetaData_LoopData : public UAnimMetaData, public IAnimMetaDataMovementInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditInstanceOnly, Category = "Loop Data")
	uint8 LoopCount = 1;
	UPROPERTY(EditInstanceOnly, Category = "Loop Data")
	TObjectPtr < UCurveVector> MoveData{};

	uint8 TempCount{};
	TimerClass Timer{};
	TObjectPtr<ACharacter> OwnerCharacter{};

	float StartLength{};
	float LoopLength{};
	float TotalLoopLength{};
	float EndLength{};
	float TotalLength{};
	bool IsMoving{};

public:
	//virtual void StartMetaData_Implementation(ACharacter* pOwner) override;

	virtual void InitUpdateMetaData_Implementation(ACharacter* pOwner, float StartLength, float LoopLength, float EndLength) override;
	virtual void UpdateMetaData_Implementation(float DeltaTime) override;
	virtual bool IsMoveEnd_Implementation() const override { return IsMoving; }
	virtual bool IsLoopingEnd_Implementation() const override { return TempCount >= LoopCount; }
};
