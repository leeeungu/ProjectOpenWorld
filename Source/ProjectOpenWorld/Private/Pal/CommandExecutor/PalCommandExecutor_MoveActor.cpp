#include "Pal/CommandExecutor/PalCommandExecutor_MoveActor.h"
#include "GameFramework/Character.h"
#include "Pal/Controller/PalAIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Pal/Component/PalCommandComponent.h"

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
		if (OwnerController->MoveToActor(Command.pTarget, 40.0f) == false)
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
	const FPalCommand* Command = OwnerCommandComp->GetCurrentCommand_C();
	if (!OwnerCommandComp->IsValidCommand() || Command->CommandKind != EPalCommandKind::Move || Command->SubCommandType != (uint8)ESubMoveType::Actor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Executor__MoveLocation :: not slef command"));
		return;
	}
	if (OwnerController)
	{
		OwnerController->ReceiveMoveCompleted.RemoveDynamic(this, &UPalCommandExecutor_MoveActor::FinishMove);
		EndCommand();
	}
}