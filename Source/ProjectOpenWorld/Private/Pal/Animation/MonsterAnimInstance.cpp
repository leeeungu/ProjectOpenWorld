#include "Pal/Animation/MonsterAnimInstance.h"
#include "Creature/Character/BaseMonster.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Pal/Component/PalCommandComponent.h"

void UMonsterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	OwnerPalCreature = Cast<ABaseMonster>(TryGetPawnOwner());
	if (!OwnerPalCreature)
		return;
	CommandComponent = OwnerPalCreature->GetCommandComponent();
}

void UMonsterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (OwnerPalCreature && CommandComponent && CommandComponent->GetCurrentCommand_C())
	{
	CurrentCommandKind = CommandComponent->GetCurrentCommand_C()->CommandKind;
	SubCommandType = CommandComponent->GetCurrentCommand_C()->SubCommandType;
	}
	//bIsStun = OwnerPalCreature->IsStunned();
}
