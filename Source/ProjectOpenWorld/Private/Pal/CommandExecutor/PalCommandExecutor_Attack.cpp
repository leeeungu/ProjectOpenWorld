#include "Pal/CommandExecutor/PalCommandExecutor_Attack.h"
#include "Creature/Character/BaseCreature.h"
#include "Pal/Controller/PalAIController.h"
#include "Navigation/PathFollowingComponent.h"

void UPalCommandExecutor_Attack::Initialize(UPalCommandComponent* CommandComp)
{
	UPalCommandExecutorBase::Initialize(CommandComp);
	if (OwnerCommandComp == nullptr)
		return;
	OwnerPal = Cast<ABaseCreature>(OwnerCommandComp->GetOwner());
	if (OwnerPal)
	{
		OwnerController = Cast<APalAIController>(OwnerPal->GetController());
	}
}


void UPalCommandExecutor_Attack::StartCommand(const FPalCommand& Command)
{
	if (Command.CommandKind != EPalCommandKind::Attack || !Command.pTarget || bStartedAttacking)
	{
		return;
	}
	if (!OwnerController)
	{
		return;
	}
	bStartedAttacking = true;
	OwnerController->ReceiveMoveCompleted.AddDynamic(this, &UPalCommandExecutor_Attack::FinishMove);
	if (OwnerController->MoveToLocation(Command.pTarget->GetActorLocation(), 40.0f) == false)
	{
		EndAttack();
	}
}

void UPalCommandExecutor_Attack::Abort()
{
	if (bStartedAttacking == false)
		return;
	bStartedAttacking = false;
	if (OwnerPal)	
	{
		OwnerPal->SetActionStarted(false);
		//OwnerPal->SetAttackData(false);
	}
	if (OwnerController)
	{
		OwnerController->StopMovement();
		OwnerController->ReceiveMoveCompleted.RemoveDynamic(this, &UPalCommandExecutor_Attack::FinishMove);
	}
}

void UPalCommandExecutor_Attack::EndAttack()
{
	Abort();
	EndCommand();
}

void UPalCommandExecutor_Attack::EndAttackComponent()
{

}

void UPalCommandExecutor_Attack::FinishMove(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (!bStartedAttacking)
		return;
	const FPalCommand* Command = OwnerCommandComp->GetCurrentCommand_C();
	if (!OwnerCommandComp->IsValidCommand() || Command->CommandKind != EPalCommandKind::Attack || Command->SubCommandType == 0 || !Command->pTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("Executor_Attack :: Can't Attack"));
	}
	if (OwnerPal )
	{
		OwnerPal->SetActionStarted(true);
		//OwnerPal->SetAttackData(true, Command->pTarget, Command->SubCommandType);
		return;
	}
	EndAttack();
}

