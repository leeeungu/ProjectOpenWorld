#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimMetaData.h"
#include "GameBase/Interface/AnimMetaDataInterface.h"
#include "GameBase/Class/TimerClass.h"
#include "AnimMetaData_LoopData.generated.h"

class ACharacter;

UCLASS()
class PROJECTOPENWORLD_API UAnimMetaData_LoopData : public UAnimMetaData, public IAnimMetaDataUpdateInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditInstanceOnly, Category = "Loop Data")
	uint8 LoopCount = 1;
	UPROPERTY(EditInstanceOnly, Category = "Loop Data")
	float MoveSpeed = 100.0f;
	uint8 TempCount{};
	float AnimationPlayLength{};
	TimerClass Timer{};
	TObjectPtr<ACharacter> OwnerCharacter{};
public:
	//virtual void StartMetaData_Implementation(ACharacter* pOwner) override;

	virtual void InitUpdateMetaData_Implementation(ACharacter* pOwner, float UpdateLength) override;
	virtual void UpdateMetaData_Implementation(float DeltaTime) override;
	virtual bool IsLoopingEnd_Implementation() const override { return TempCount <= 0; }
};
