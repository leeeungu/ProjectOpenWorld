#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	float ClimbDirection{};
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	float ClimbSpeed{};

public:
	void SetClimbDirection(float Direction) {
		ClimbDirection = Direction;
	}
	void SetClimbSpeed(float Speed) {
		ClimbSpeed = Speed;
	}
};	
