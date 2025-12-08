#include "Creature/Character/BaseMonster.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Pal/Component/PalAttackComponent.h"
#include "Pal/Controller/PalMonsterController.h"
#include "Pal/Component/PalMonsterCommandComponent.h"
#include "Pal/Factory/PalCommandFunctionLibrary.h"


ABaseMonster::ABaseMonster() :
	ABaseCharacter{}
{
	AIControllerClass = APalMonsterController::StaticClass();
	CommandComponent = CreateDefaultSubobject<UPalMonsterCommandComponent>(TEXT("PalCommand"));
	AttackComponent = CreateDefaultSubobject<UPalAttackComponent>(TEXT("AttackComponent"));
	Hp = 100;
	Attack = 10.0f;
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
	if (!Other || !Other.GetObject())
		return false;
	float Damage = IAttackInterface::Execute_GetAttackValue(Other.GetObject());
	Hp -= Damage;
	if (CommandComponent->IsValidCommand())
	{
		CommandComponent->ResetCommandQue();
	}
	if (AttackComponent && Cast< AActor>(Other.GetObject()) && !CommandComponent->IsValidCommand())
	{
		UE_LOG(LogTemp, Log, TEXT("ABaseMonster :: Attack"), Hp);
		CommandComponent->PushCommand(UPalCommandFunctionLibrary::CommandAttack(this, Cast< AActor>(Other.GetObject()), ESubAttackType::Default));
	}
	UE_LOG(LogTemp, Log, TEXT("HP : %f"), Hp);
	if (Hp <= 0.f)
		Destroy();
	return true;
}
