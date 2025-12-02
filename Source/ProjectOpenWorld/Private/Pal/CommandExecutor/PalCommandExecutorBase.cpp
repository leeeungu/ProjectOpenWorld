#include "Pal/CommandExecutor/PalCommandExecutorBase.h"

void UPalCommandExecutorBase::Initialize(UPalCommandComponent* CommandComp)
{
	this->OwnerCommandComp = CommandComp;
}

void UPalCommandExecutorBase::EndCommand()
{
	if (OwnerCommandComp)
	{
		OwnerCommandComp->FinishCommand();
	}
}
