#include "Pal/CommandExecutor/PalCommandExecutor_MoveLocation.h"
#include "GameFramework/Character.h"
#include "Pal/Controller/PalAIController.h"
#include "Pal/Component/PalCommandComponent.h"

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

bool UPalCommandExecutor_MoveLocation::StartCommand(const FPalCommand& Command)
{
	if (OwnerController)
	{
		OwnerController->SetBBTargetLocation(Command.TargetLocation);
		return true;
	}
	IsCommandStarted = true;
	return false;
}

void UPalCommandExecutor_MoveLocation::Abort()
{
	if (OwnerController)
	{
		OwnerController->ResetMove();
	}
}

void UPalCommandExecutor_MoveLocation::WorkCommand()
{
	
	EndCommand();
}
bool UPalCommandExecutor_MoveLocation::CheckCommandValid()
{
	if (!OwnerCommandComp)
		return false;
	const FPalCommand* Command = OwnerCommandComp->GetCurrentCommand_C();
	if (!OwnerCommandComp->IsValidCommand() || Command->CommandKind != EPalCommandKind::Move || Command->SubCommandType != (uint8)ESubMoveType::Location)
	{
		UE_LOG(LogTemp, Warning, TEXT("Executor_MoveLocation :: not slef command"));
		return false;
	}
	return true;
}
