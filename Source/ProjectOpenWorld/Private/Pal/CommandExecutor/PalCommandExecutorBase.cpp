#include "Pal/CommandExecutor/PalCommandExecutorBase.h"
#include "Pal/Component/PalCommandComponent.h"


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
