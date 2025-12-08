#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Pal/Component/PalAttackComponent.h"
#include "Pal/Data/PalCommandData.h"
#include "PalAnimInstance.generated.h"

class ABaseCreature;
class UCharacterMovementComponent;
class UPalCommandComponent;

UCLASS()
class PROJECTOPENWORLD_API UPalAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, Category = "PalAnim")
	ABaseCreature* OwnerPalCreature{};
	UPROPERTY(BlueprintReadOnly, Category = "PalAnim")
	UCharacterMovementComponent* MovementComponent{};
	UPROPERTY(BlueprintReadOnly, Category = "PalAnim")
	UPalAttackComponent* AttackComponent{};
	UPROPERTY(BlueprintReadOnly, Category = "PalAnim")
	UPalCommandComponent* CommandComponent{};

	UPROPERTY(BlueprintReadOnly, Category = "PalAnim")
	FPalTempAttackAnim AttckAnimations{};

	UPROPERTY(BlueprintReadWrite, Category = "PalAnim")
	bool IsFalling{};
	UPROPERTY(BlueprintReadWrite, Category = "PalAnim")
	float Speed{};
	UPROPERTY(BlueprintReadWrite, Category = "PalAnim")
	float PreSpeed{};
	UPROPERTY(BlueprintReadWrite, Category = "PalAnim")
	EPalCommandKind CurrentCommandKind{};
	UPROPERTY(BlueprintReadWrite, Category = "PalAnim")
	uint8 SubCommandType{};
	UPROPERTY(BlueprintReadWrite, Category = "PalAnim")
	bool bActionStarted{};

	UPROPERTY(BlueprintReadWrite, Category = "PalAnim")
	bool bAttacking{};
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
};
