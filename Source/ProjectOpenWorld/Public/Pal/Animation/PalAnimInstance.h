#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Pal/Data/PalCommandData.h"
#include "PalAnimInstance.generated.h"

class ABaseCreature;
class UCharacterMovementComponent;
class UPalAttackComponent;
class UPalCommandComponent;

UCLASS()
class PROJECTOPENWORLD_API UPalAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
protected:
	
	ABaseCreature* OwnerPalCreature{};
	UCharacterMovementComponent* MovementComponent{};
	UPalAttackComponent* AttachmentComponent{};
	UPalCommandComponent* CommandComponent{};

	bool IsFalling{};
	float Speed{};
	float PreSpeed{};
	EPalCommandKind CurrentCommandKind{};
	uint8 SubCommandType{};
	bool bActionStarted{};

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
};
