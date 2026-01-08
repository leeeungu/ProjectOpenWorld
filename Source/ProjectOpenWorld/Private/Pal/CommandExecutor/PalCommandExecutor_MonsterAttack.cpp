#include "Pal/CommandExecutor/PalCommandExecutor_MonsterAttack.h"
#include "Creature/Character/BaseMonster.h"
#include "GameFramework/Controller.h"
#include "Pal/Component/PalAttackComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Pal/Component/PalCommandComponent.h"

void UPalCommandExecutor_MonsterAttack::Initialize(UPalCommandComponent* CommandComp)
{
	UPalCommandExecutorBase::Initialize(CommandComp);
	if (OwnerCommandComp == nullptr)
		return;
	OwnerPal = Cast<ABaseMonster>(OwnerCommandComp->GetOwner());
	if (OwnerPal)
	{
		AttackComponent = OwnerPal->GetAttackComponent();
	}
	if (AttackComponent)
	{
		AttackComponent->OnPalAttackEnd.AddUniqueDynamic(this, &UPalCommandExecutor_MonsterAttack::EndAttack);
	}
}


bool UPalCommandExecutor_MonsterAttack::StartCommand(const FPalCommand& Command)
{
	if (!AttackComponent || Command.CommandKind != EPalCommandKind::Attack || !Command.pTarget.IsValid() || bStartedAttacking)
	{
		return false;
	}
	bStartedAttacking = true;
	if (AttackComponent)
	{
		//FPalAttackDataTable NewAttackData{};
		////NewAttackData.TargetActor = Command.pTarget.Get();
		//NewAttackData.AttackSlot = (ESubAttackType)Command.SubCommandType;
		//AttackComponent->SetAttackData(NewAttackData);
		AttackComponent->SetAttackTarget(Command.pTarget.Get());
		//AttackComponent->StartAttack();
		IsCommandStarted = true;
		return true;
	}
	EndAttack();
	return false;
}

void UPalCommandExecutor_MonsterAttack::Abort()
{
	bStartedAttacking = false;
	if (OwnerPal)
	{
		if (AttackComponent)
		{
			AttackComponent->EndAttack();
		}
	}
	if (OwnerPal->GetController())
	{
		OwnerPal->GetController()->StopMovement();
	}
}

void UPalCommandExecutor_MonsterAttack::EndAttack()
{
	if (bStartedAttacking == false)
		return;
	Abort();
	EndCommand();
}


void UPalCommandExecutor_MonsterAttack::WorkCommand()
{
}

bool UPalCommandExecutor_MonsterAttack::CheckCommandValid()
{
	return true;
}
