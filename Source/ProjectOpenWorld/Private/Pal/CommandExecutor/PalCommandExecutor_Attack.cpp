#include "Pal/CommandExecutor/PalCommandExecutor_Attack.h"
#include "Creature/Character/BaseCreature.h"
#include "GameFramework/Controller.h"
#include "Pal/Component/PalAttackComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Pal/Component/PalCommandComponent.h"

void UPalCommandExecutor_Attack::Initialize(UPalCommandComponent* CommandComp)
{
	UPalCommandExecutorBase::Initialize(CommandComp);
	if (OwnerCommandComp == nullptr)
		return;
	OwnerPal = Cast<ABaseCreature>(OwnerCommandComp->GetOwner());
	if (OwnerPal)
	{
		AttackComponent = OwnerPal->GetAttackComponent();
	}
	if (AttackComponent)
	{
		AttackComponent->OnPalAttackEnd.AddUniqueDynamic(this, &UPalCommandExecutor_Attack::EndAttack);
	}
}


bool UPalCommandExecutor_Attack::StartCommand(const FPalCommand& Command)
{
	if (!AttackComponent || Command.CommandKind != EPalCommandKind::Attack || !Command.pTarget.IsValid() || bStartedAttacking)
	{
		return false;
	}
	bStartedAttacking = true;
	OwnerPal->SetActionStarted(true);
	if (AttackComponent)
	{
		//FPalAttackDataTable NewAttackData{};
		////NewAttackData.TargetActor = Command.pTarget.Get();
		//NewAttackData.AttackSlot = (ESubAttackType)Command.SubCommandType;
		AttackComponent->SetAttackTarget(Command.pTarget.Get());
		//AttackComponent->SetAttackData(NewAttackData);
		//AttackComponent->StartAttack();
		IsCommandStarted = true;
		return true;
	}
	EndAttack();
	return false;
}

void UPalCommandExecutor_Attack::Abort()
{
	if (bStartedAttacking == false)
		return;
	bStartedAttacking = false;
	if (OwnerPal)
	{
		OwnerPal->SetActionStarted(false);
		if (OwnerPal->GetController())
		{
			OwnerPal->GetController()->StopMovement();
		}
	}
	if (AttackComponent)
	{
		AttackComponent->EndAttack();
	}
}

void UPalCommandExecutor_Attack::WorkCommand()
{
}

bool UPalCommandExecutor_Attack::CheckCommandValid()
{
	return true;
}

void UPalCommandExecutor_Attack::EndAttack()
{
	Abort();
	EndCommand();
}

