#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerAnimationComponent.generated.h"

class ACharacter;

UENUM(BlueprintType)
enum class EAnimationState : uint8
{
	Travel,
	Climb
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UPlayerAnimationComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	EAnimationState AnimationState{};
	TObjectPtr<ACharacter> OwnerCharacter{};

	FHitResult ClimbHeadHit{};
	FHitResult ClimbForwardHit{};
	FHitResult ClimbDownHit{};
	FHitResult ClimbUpHit{};
	bool bHeadHit{ false };
	bool bCanClimb{ false };
	bool bCanClimbDown{ false };
	bool bCanClimbUp{ false };
	TArray<AActor*> IgnoreArray{};
public:	
	UPlayerAnimationComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure, Category = "Animation")
	bool IsClimbing() const { return EAnimationState::Climb == AnimationState; }
	UFUNCTION(BlueprintPure, Category = "Animation")
	EAnimationState GetAnimationState() const { return AnimationState; }

	UFUNCTION(BlueprintPure, Category = "Animation")
	bool CanClimb() const { return bCanClimb; }
	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool StartClimb();
	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool StartTravel();

	//void MoveClimb(const FInputActionValue& Value);

protected:
	virtual void BeginPlay() override;

	bool ClimbTick(float DeltaTime);

};
