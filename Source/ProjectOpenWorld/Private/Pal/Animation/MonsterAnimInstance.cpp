#include "Pal/Animation/MonsterAnimInstance.h"

#include "Creature/Character/BaseMonster.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Pal/Component/PalCommandComponent.h"

void UMonsterAnimInstance::NativeInitializeAnimation()
{
	UAnimInstance::NativeInitializeAnimation();
	OwnerPalCreature = Cast<ABaseMonster>(TryGetPawnOwner());
	if (!OwnerPalCreature)
		return;
	MovementComponent = OwnerPalCreature->GetCharacterMovement();
	AttackComponent = OwnerPalCreature->GetAttackComponent();
	CommandComponent = OwnerPalCreature->GetCommandComponent();
}

void UMonsterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	UAnimInstance::NativeUpdateAnimation(DeltaSeconds);
	if (!OwnerPalCreature || !MovementComponent || !CommandComponent || !AttackComponent)
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
	AttckAnimations = AttackComponent->GetAnimation();
	bAttacking = AttackComponent->GetAttacking();
	if (CurrentCommandKind == EPalCommandKind::Work && SubCommandType == (uint8)ESubWorkType::Transport && bActionStarted)
	{
		Speed = FMath::Clamp(Speed, 0.0f, 0.46f);
	}	
	PreSpeed = Speed;
}
