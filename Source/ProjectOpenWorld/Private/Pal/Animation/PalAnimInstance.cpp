#include "Pal/Animation/PalAnimInstance.h"
#include "Creature/Character/BaseCreature.h"
#include "Pal/Component/PalCommandComponent.h"

void UPalAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	OwnerPalCreature = Cast<ABaseCreature>(TryGetPawnOwner());
	if (!OwnerPalCreature)
		return;
	CommandComponent = OwnerPalCreature->GetCommandComponent();
}

void UPalAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!OwnerPalCreature || !CommandComponent )
	{
		return;
	}
	CurrentCommandKind = CommandComponent->GetCurrentCommand_C()->CommandKind;
	SubCommandType = CommandComponent->GetCurrentCommand_C()->SubCommandType;
	bActionStarted = OwnerPalCreature->GetActionStarted();
	//AttckAnimations = AttackComponent->GetAttackAnimation();
	if (CurrentCommandKind == EPalCommandKind::Work && SubCommandType == (uint8)ESubWorkType::Transport && bActionStarted)
	{
		MoveSpeed = FMath::Clamp(MoveSpeed, 0.0f, 0.46f);
	}
}
