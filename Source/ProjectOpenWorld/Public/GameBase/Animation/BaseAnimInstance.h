#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BaseAnimInstance.generated.h"

class UCharacterMovementComponent;
class UAnimLoopObject;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMontageEvent);

UCLASS()
class PROJECTOPENWORLD_API UBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "MontageManager")
	TArray<TObjectPtr<UAnimMontage>> MontageArray{};
	UPROPERTY()
	TObjectPtr < UAnimMontage> CurrentMontage{};
	UPROPERTY()
	TObjectPtr < UAnimLoopObject> LoopObject{};

	UPROPERTY(BlueprintReadOnly, Category = "PalAnim")
	TObjectPtr<UCharacterMovementComponent> MovementComponent{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalAnim")
	bool IsFalling{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalAnim")
	float MoveSpeed{};

	int CurrentMontageIndex{};
	bool bIsPlayingMontage{};
public:
	UPROPERTY(BlueprintAssignable, Category = "MontageManager")
	FOnMontageEvent OnMontageQueueEnd;
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UFUNCTION()
	void OnMontageStartedEvent(UAnimMontage* Montage);
	UFUNCTION()
	void OnMontageBlendingOutEvent(UAnimMontage* Montage, bool bInterrupted);

	bool CanPlayMontage();
	void OnMontageEnd();

	void PlayMontage();
	bool IsLoop() const;
public:
	UFUNCTION(BlueprintCallable, Category = "MontageManager")
	void ChangeMontageArray(const TArray<UAnimMontage*>& NewMontageArray);

	UFUNCTION(BlueprintCallable, Category = "MontageManager")
	bool PlayMontageQueue();
	UFUNCTION(BlueprintCallable, Category = "MontageManager")
	void StopMontageQueue();

	UFUNCTION(BlueprintPure, Category = "MontageManager")
	bool IsPlayingMontage() const { return bIsPlayingMontage; }
};
