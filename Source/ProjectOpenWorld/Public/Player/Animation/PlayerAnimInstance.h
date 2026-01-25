#pragma once

#include "CoreMinimal.h"
#include "GameBase/Animation/BaseAnimInstance.h"
#include "PlayerAnimInstance.generated.h"

class ABasePlayer;
class UCharacterMovementComponent;
enum class EAnimationState : uint8;
class UPlayerAnimationComponent;
enum class EPlayerState : uint8;

UCLASS()
class PROJECTOPENWORLD_API UPlayerAnimInstance : public UBaseAnimInstance
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	TObjectPtr< ABasePlayer> OwnerPlayer{};

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	TObjectPtr <UPlayerAnimationComponent> AnimationComponent{};
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	FVector Velocity{};
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (ClampMin = -180.0f, ClampMax = 180.0f))
	float DesireYaw{};

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	EAnimationState AnimationState{};
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	bool IsClimbing{ false };

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	FVector LeftHandNormal{};
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	FVector RightHandNormal{};
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	FVector LeftFootNormal{};
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	FVector RightFootNormal{};

	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (ClampMin = -180.0f, ClampMax = 180.0f))
	float ClimbDirection{};
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float ClimbSpeed{};

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	EPlayerState CurrentPlayerState{};

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void SetClimbDirection(float Direction) {
		ClimbDirection = FMath::Clamp(Direction, -180.0f, 180.0f);
	}
	void SetClimbSpeed(float Speed) {
		ClimbSpeed = FMath::Clamp(Speed, 0.0f, 1.0f);
	}

private:
	UFUNCTION()
	void SetPlayerState(EPlayerState NewState);
};	
