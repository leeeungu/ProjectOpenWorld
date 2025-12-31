#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimMetaData.h"
#include "GameBase/Interface/AnimMetaDataInterface.h"
#include "GameBase/Class/TimerClass.h"
#include "AnimMetaData_LoopData.generated.h"

class ACharacter;
class UCurveVector;

UCLASS()
class PROJECTOPENWORLD_API UAnimMetaData_LoopData : public UAnimMetaData, public IAnimMetaDatLoopInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditInstanceOnly, Category = "Loop Data")
	uint8 LoopCount = 1;
	UPROPERTY(EditInstanceOnly, Category = "Loop Data")
	TObjectPtr < UCurveVector> MoveData{};

	uint8 TempCount{};
	TimerClass Timer{};

	float LoopLength{};
	float StartLength{};
	float TotalLength{};
	//TObjectPtr<ACharacter> OwnerCharacter{};
	//float StartLength{};
	//float LoopLength{};
	//float TotalLoopLength{};
	//float EndLength{};
	//float TotalLength{};
	//bool IsMoving{};

public:
	uint8 GetLoopCount() const { return LoopCount; }
	//virtual void StartMetaData_Implementation(ACharacter* pOwner) override;

	virtual void InitLoopMetaData_Implementation(ACharacter* pOwner, float StartLength, float LoopLength) override;
	virtual void LoopUpdate_Implementation(float DeltaTime) override;
	virtual bool IsLoopingEnd_Implementation() const override { return TempCount >= LoopCount ; }
};
