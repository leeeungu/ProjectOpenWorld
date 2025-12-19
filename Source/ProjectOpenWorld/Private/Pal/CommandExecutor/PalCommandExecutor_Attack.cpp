#include "Pal/CommandExecutor/PalCommandExecutor_Attack.h"
#include "Creature/Character/BaseCreature.h"
#include "Pal/Controller/PalAIController.h"
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
		OwnerController = Cast<APalAIController>(OwnerPal->GetController());
		AttackComponent = OwnerPal->GetAttackComponent();
	}
	if (AttackComponent)
	{
		AttackComponent->OnPalAttackEnd.AddUniqueDynamic(this, &UPalCommandExecutor_Attack::EndAttack);
	}
}


bool UPalCommandExecutor_Attack::StartCommand(const FPalCommand& Command)
{
	if (!AttackComponent || Command.CommandKind != EPalCommandKind::Attack || !Command.pTarget || bStartedAttacking)
	{
		return false;
	}
	if (!OwnerController)
	{
		return false;
	}
	bStartedAttacking = true;
	OwnerPal->SetActionStarted(true);
	if (AttackComponent)
	{
		FPalAttackData NewAttackData{};
		NewAttackData.TargetActor = Command.pTarget;
		NewAttackData.AttackSlot = (ESubAttackType)Command.SubCommandType;
		AttackComponent->SetAttackData(NewAttackData);
		AttackComponent->StartAttack();
		return true;
	}
	EndAttack();
	return false;
}

void UPalCommandExecutor_Attack::Abort()
{
	bStartedAttacking = false;
	if (OwnerPal)	
	{
		OwnerPal->SetActionStarted(false);
		if (AttackComponent)
		{
			AttackComponent->EndAttack();
		}
	}
	if (OwnerController)
	{
		OwnerController->StopMovement();
	}
}

void UPalCommandExecutor_Attack::EndAttack()
{
	if (bStartedAttacking == false)
		return;
	Abort();
	EndCommand();
}

