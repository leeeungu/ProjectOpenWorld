#include "Pal/Animation/PalAnimInstance.h"
#include "Creature/Character/BaseCreature.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Pal/Component/PalAttackComponent.h"
#include "Pal/Component/PalCommandComponent.h"

void UPalAnimInstance::NativeInitializeAnimation()
{
	OwnerPalCreature = Cast<ABaseCreature>(TryGetPawnOwner());
	MovementComponent = OwnerPalCreature->GetCharacterMovement();
	AttachmentComponent = OwnerPalCreature->GetAttackComponent();
	CommandComponent = OwnerPalCreature->GetCommandComponent();
}

void UPalAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (!OwnerPalCreature || !MovementComponent || !CommandComponent)
	{
		return;
	}
	IsFalling = MovementComponent->IsFalling();
	Speed = 0.0f;
	if (MovementComponent->GetMaxSpeed() > 0)
		Speed = MovementComponent->Velocity.Length() / MovementComponent->GetMaxSpeed();
	Speed = FMath::Clamp(Speed, 0.0f, 1.0f);
	CurrentCommandKind = CommandComponent->GetCurrentCommand_C()->CommandKind;
	SubCommandType = CommandComponent->GetCurrentCommand_C()->SubCommandType;
	bActionStarted = OwnerPalCreature->GetActionStarted();
	
	if (CurrentCommandKind == EPalCommandKind::Work && SubCommandType == (uint8)ESubWorkType::Transport && bActionStarted)
	{
		Speed = FMath::Clamp(Speed, 0.0f, 0.46f);
	}
	PreSpeed = Speed;
}
