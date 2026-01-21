#include "Pal/CommandExecutor/PalCommandExecutor_MoveActor.h"
#include "GameFramework/Character.h"
#include "Pal/Controller/PalAIController.h"
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

bool UPalCommandExecutor_MoveActor::StartCommand(const FPalCommand& Command)
{
	if (OwnerController && Command.pTarget.IsValid())
	{
		OwnerController->SetBBTargetActor(Command.pTarget.Get());
		return true;
	}
	IsCommandStarted = true;
	return false;
}

void UPalCommandExecutor_MoveActor::WorkCommand()
{
	EndCommand();
}
bool UPalCommandExecutor_MoveActor::CheckCommandValid()
{
	if (!OwnerCommandComp)
		return false;
	const FPalCommand* Command = OwnerCommandComp->GetCurrentCommand_C();
	if (!OwnerCommandComp->IsValidCommand() || Command->CommandKind != EPalCommandKind::Move || Command->SubCommandType != (uint8)ESubMoveType::Actor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Executor_MoveActor :: not slef command"));
		return false;
	}
	return true;
}
void UPalCommandExecutor_MoveActor::Abort()
{
	if (OwnerController)
	{
		OwnerController->ResetMove();
	}
}
