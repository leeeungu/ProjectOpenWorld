#include "Pal/CommandExecutor/PalCommandExecutor_Attack.h"
#include "Creature/Character/BaseCreature.h"
#include "Pal/Controller/PalAIController.h"
#include "Pal/Component/PalAttackComponent.h"
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
		AttackComponent = OwnerPal->GetAttackComponent();
	}
	if (AttackComponent)
	{
		AttackComponent->OnPalAttackEnd.AddDynamic(this, &UPalCommandExecutor_Attack::EndAttack);
	}
}


void UPalCommandExecutor_Attack::StartCommand(const FPalCommand& Command)
{
	if (!AttackComponent || Command.CommandKind != EPalCommandKind::Attack || !Command.pTarget || bStartedAttacking)
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
		OwnerController->ReceiveMoveCompleted.RemoveDynamic(this, &UPalCommandExecutor_Attack::FinishMove);
	}
}

void UPalCommandExecutor_Attack::EndAttack()
{
	if (bStartedAttacking == false)
		return;
	Abort();
	EndCommand();
}

void UPalCommandExecutor_Attack::FinishMove(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (!bStartedAttacking)
		return;
	const FPalCommand* Command = OwnerCommandComp->GetCurrentCommand_C();
	if (!OwnerPal || !OwnerCommandComp->IsValidCommand() || Command->CommandKind != EPalCommandKind::Attack || Command->SubCommandType == 0 || !Command->pTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("Executor_Attack :: Can't Attack"));
	}
	else
	{
		OwnerPal->SetActionStarted(true);
		if (AttackComponent)
		{
			FPalAttackData NewAttackData{};
			NewAttackData.TargetActor = Command->pTarget;
			NewAttackData.AttackSlot = Command->SubCommandType;
			AttackComponent->SetAttackData(NewAttackData);
			AttackComponent->StartAttack();
		}
		return;
	}
	EndAttack();
}

