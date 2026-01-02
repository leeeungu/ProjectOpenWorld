#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameBase/Interface/MontageQueueInterface.h"
#include "BaseAnimInstance.generated.h"

class UCharacterMovementComponent;
class UAnimLoopObject;

UCLASS()
class PROJECTOPENWORLD_API UBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
private:
	TScriptInterface<IMontageQueueInterface> MontageQueue{};
	UPROPERTY()
	TObjectPtr < UAnimMontage> CurrentMontage{};
	UPROPERTY()
	TObjectPtr < UAnimLoopObject> LoopObject{};
protected:

	UPROPERTY(BlueprintReadOnly, Category = "PalAnim")
	TObjectPtr<UCharacterMovementComponent> MovementComponent{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalAnim")
	bool IsFalling{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalAnim")
	float MoveSpeed{};
private:
	UFUNCTION()
	void OnMontageStartedFunc(UAnimMontage* Montage);
	UFUNCTION()
	void OnMontageBlendingOutFunc(UAnimMontage* Montage, bool bInterrupted);

public:
	bool IsLoop() const;
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	void SetMontageQueueInterface(TScriptInterface<IMontageQueueInterface> Interface);
	void PlayMontageQueue();
	bool MontageIsPlay(UAnimMontage* Montage);
};
