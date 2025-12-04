#include "Pal/CommandExecutor/PalCommandExecutor_Mining.h"
#include "Creature/Character/BaseCreature.h"
#include "Pal/Controller/PalAIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Pal/Component/PalCommandComponent.h"

void UPalCommandExecutor_Mining::Initialize(UPalCommandComponent* CommandComp)
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

void UPalCommandExecutor_Mining::StartCommand(const FPalCommand& Command)
{
	if (!OwnerPal || !OwnerController || !Command.pTarget)
		return;

	if (OwnerController)
	{
		bStartedMining = true;
		OwnerController->ReceiveMoveCompleted.AddDynamic(this, &UPalCommandExecutor_Mining::FinishMove);
		if (OwnerController->MoveToLocation(Command.pTarget->GetActorLocation(), 40.0f) == false)
		{
			EndMining();
		}
	}
}

void UPalCommandExecutor_Mining::Abort()
{
	bStartedMining = false;
	if (OwnerPal)
	{
		OwnerPal->SetActionStarted(false);
	}
	if (OwnerController)
	{
		OwnerController->StopMovement();
		OwnerController->ReceiveMoveCompleted.RemoveDynamic(this, &UPalCommandExecutor_Mining::FinishMove);
	}
}

void UPalCommandExecutor_Mining::EndMining()
{
	Abort();
	EndCommand();
}

void UPalCommandExecutor_Mining::FinishMove(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (!bStartedMining)
		return;
	const FPalCommand* Command = OwnerCommandComp->GetCurrentCommand_C();
	if (!OwnerCommandComp->IsValidCommand() || Command->CommandKind != EPalCommandKind::Work || Command->SubCommandType != (uint8)ESubWorkType::Mining|| !Command->pTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("Executor_Transport :: Can't move to Destination"));
	}
	if (OwnerPal && FVector::Distance(Command->pTarget->GetActorLocation(), OwnerPal->GetActorLocation()) <= 120.0f)
	{
		OwnerPal->SetActionStarted(true);
		return;
	}
	EndMining();
}