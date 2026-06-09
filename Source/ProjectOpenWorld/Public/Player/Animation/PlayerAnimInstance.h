#pragma once

#include "CoreMinimal.h"
#include "GameBase/Animation/BaseAnimInstance.h"
#include "GameBase/Animation/AnimActionPlayType.h"
#include "PlayerAnimInstance.generated.h"

class ABasePlayer;
class UCharacterMovementComponent;
enum class EAnimationState : uint8;
class UPlayerAnimationComponent;
enum class EPlayerState : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnimSectionEvent);

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


	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	EAnimationSectionType CurrentAnimationSection{};
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimSequence> StartAnim{};
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimSequence> LoopAnim{};
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimSequence> EndAnim{};

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimSequence> ArchitectStartAnim{};
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimSequence> ArchitectLoopAnim{};
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimSequence> ArchitectEndAnim{};

	bool bSetAnimation{};
public:
	UPROPERTY(BlueprintAssignable)
	FOnAnimSectionEvent OnStartAnimSection{};
	UPROPERTY(BlueprintAssignable)
	FOnAnimSectionEvent OnStartLoopAnimSection{};
	UPROPERTY(BlueprintAssignable)
	FOnAnimSectionEvent OnFinishAnimSection{};

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void SetClimbDirection(float Direction) {
		ClimbDirection = FMath::Clamp(Direction, -180.0f, 180.0f);
	}
	void SetClimbSpeed(float Speed) {
		ClimbSpeed = FMath::Clamp(Speed, 0.0f, 1.0f);
	}
	UFUNCTION(BlueprintCallable)
	 bool StartAnimSection();
	
	UFUNCTION(BlueprintCallable)
	 bool EndAnimSection();
	UFUNCTION(BlueprintCallable)
	 bool ResetAnimSection();

protected:
	UFUNCTION()
	void AnimNotify_FinishAnimSection();
	UFUNCTION()
	void AnimNotify_LoopAnimSection();

public:
	UFUNCTION(BlueprintPure)
	FORCEINLINE EAnimationSectionType GetAnimSectionType() const { return CurrentAnimationSection; }

	bool SetAnimationSequences(UAnimSequence* Start, UAnimSequence* Loop, UAnimSequence* End);


	bool IsSetAnimation() const;
	bool IsPlayerAnimationSeq() const { return CurrentAnimationSection != EAnimationSectionType::None; }
	UFUNCTION(BlueprintCallable)
	bool SetArchitectAnimSequence();

private:
	UFUNCTION()
	void SetPlayerState(EPlayerState NewState);
};	
