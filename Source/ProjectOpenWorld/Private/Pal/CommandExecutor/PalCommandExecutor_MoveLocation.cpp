#include "Pal/CommandExecutor/PalCommandExecutor_MoveLocation.h"
#include "GameFramework/Character.h"
#include "Pal/Controller/PalAIController.h"
#include "Pal/Component/PalCommandComponent.h"
#include "Navigation/PathFollowingComponent.h"

void UPalCommandExecutor_MoveLocation::Initialize(UPalCommandComponent* CommandComp)
{
	UPalCommandExecutorBase::Initialize(CommandComp);
	if (OwnerCommandComp)
		OwnerPal = Cast<ACharacter>(OwnerCommandComp->GetOwner());
	if (OwnerPal)
	{
		OwnerController = Cast<APalAIController>(OwnerPal->GetController());
	}
}

void UPalCommandExecutor_MoveLocation::StartCommand(const FPalCommand& Command)
{
	if (OwnerController)
	{
		OwnerController->ReceiveMoveCompleted.AddDynamic(this, &UPalCommandExecutor_MoveLocation::FinishMove);
		if (OwnerController->MoveToLocation(Command.TargetLocation,40.0f) == false)
		{
			OwnerController->ReceiveMoveCompleted.RemoveDynamic(this, &UPalCommandExecutor_MoveLocation::FinishMove);
			EndCommand();
		}
	}
}

void UPalCommandExecutor_MoveLocation::Abort()
{
	if (OwnerController)
	{
		OwnerController->ReceiveMoveCompleted.RemoveDynamic(this, &UPalCommandExecutor_MoveLocation::FinishMove);
		OwnerController->StopMovement();
	}
}


void UPalCommandExecutor_MoveLocation::FinishMove(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (OwnerController)
	{
		OwnerController->ReceiveMoveCompleted.RemoveDynamic(this, &UPalCommandExecutor_MoveLocation::FinishMove);
		EndCommand();
	}
}