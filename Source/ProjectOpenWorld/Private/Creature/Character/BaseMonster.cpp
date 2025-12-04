#include "Creature/Character/BaseMonster.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Pal/Component/PalAttackComponent.h"
#include "Pal/Controller/PalAIController.h"
#include "Pal/Component/PalMonsterCommandComponent.h"

ABaseMonster::ABaseMonster() :
	ABaseCharacter{}
{
	AIControllerClass = APalAIController::StaticClass();
	CommandComponent = CreateDefaultSubobject<UPalMonsterCommandComponent>(TEXT("PalCommand"));
	AttackComponent = CreateDefaultSubobject<UPalAttackComponent>(TEXT("AttackComponent"));
}

void ABaseMonster::ReceiveCommand_Implementation(FPalCommand Command)
{
	if (CommandComponent)
	{
		CommandComponent->PushCommand(Command);
	}
}

float ABaseMonster::GetAttackValue_Implementation() const
{
	return Attack;
}

void ABaseMonster::SetAttackValue_Implementation(float NewValue)
{
	Attack = NewValue;
}

void ABaseMonster::RetAttackValue_Implementation()
{
	Attack = 1.0f;
}

bool ABaseMonster::DamagedCharacter_Implementation(const TScriptInterface<IAttackInterface>& Other)
{
	return false;
}
