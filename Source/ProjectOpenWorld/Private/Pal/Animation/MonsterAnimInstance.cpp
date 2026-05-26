#include "Pal/Animation/MonsterAnimInstance.h"
#include "Pal/Character/PalBaseMonster.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Pal/Component/PalCommandComponent.h"

void UMonsterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	OwnerPalCreature = Cast<APalBaseMonster>(TryGetPawnOwner());
	if (!OwnerPalCreature)
		return;
	//CommandComponent = OwnerPalCreature->GetCommandComponent();
}

void UMonsterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	//if (OwnerPalCreature && CommandComponent && CommandComponent->GetCurrentCommand_C())
	//{
	//CurrentCommandKind = CommandComponent->GetCurrentCommand_C()->CommandKind;
	//SubCommandType = CommandComponent->GetCurrentCommand_C()->SubCommandType;
	//}
	//bIsStun = OwnerPalCreature->IsStunned();
}
