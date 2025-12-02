#include "Pal/CommandExecutor/PalCommandExecutor_MoveActor.h"
#include "GameFramework/Character.h"
#include "Pal/Controller/PalAIController.h"
#include "Navigation/PathFollowingComponent.h"

void UPalCommandExecutor_MoveActor::Initialize(UPalCommandComponent* CommandComp)
{
	UPalCommandExecutorBase::Initialize(CommandComp);
	if (OwnerCommandComp)
		OwnerPal = Cast<ACharacter>(OwnerCommandComp->GetOwner());
	if (OwnerPal)
	{
		OwnerController = Cast<APalAIController>(OwnerPal->GetController());
	}
}

void UPalCommandExecutor_MoveActor::StartCommand(const FPalCommand& Command)
{
	if (OwnerController)
	{
		OwnerController->ReceiveMoveCompleted.AddDynamic(this, &UPalCommandExecutor_MoveActor::FinishMove);
		if (OwnerController->MoveToActor(Command.pTarget) == false)
		{
			OwnerController->ReceiveMoveCompleted.RemoveDynamic(this, &UPalCommandExecutor_MoveActor::FinishMove);
			EndCommand();
		}
	}
}

void UPalCommandExecutor_MoveActor::Abort()
{
	if (OwnerController)
	{
		OwnerController->ReceiveMoveCompleted.RemoveDynamic(this, &UPalCommandExecutor_MoveActor::FinishMove);
		OwnerController->StopMovement();
	}
}
void UPalCommandExecutor_MoveActor::FinishMove(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (OwnerController)
	{
		OwnerController->ReceiveMoveCompleted.RemoveDynamic(this, &UPalCommandExecutor_MoveActor::FinishMove);
		EndCommand();
	}
}