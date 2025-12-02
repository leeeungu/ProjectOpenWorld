#include "Pal/CommandExecutor/PalCommandExecutor_Architecture.h"
#include "Building/Actor/BuildingActor.h"
#include "Building/Component/BuildingProgress.h"
#include "Creature/Character/BaseCreature.h"
#include "Pal/Controller/PalAIController.h"
#include "Navigation/PathFollowingComponent.h"

void UPalCommandExecutor_Architecture::Initialize(UPalCommandComponent* CommandComp)
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

void UPalCommandExecutor_Architecture::StartCommand(const FPalCommand& Command)
{
	TargetBuilding = Cast< ABuildingActor>(Command.pTarget);
	if (!TargetBuilding || TargetBuilding->GetBuildingProgress()->IsBuildingEnd() || !OwnerPal)
		return;
	
	UE_LOG(LogTemp, Warning, TEXT("Start Architecture :: %s"), *OwnerCommandComp->GetOwner()->GetFName().ToString());
	bActionStart = true;
	TargetBuilding->GetBuildingProgress()->onBuildingEnd.AddDynamic(this, &UPalCommandExecutor_Architecture::EndBuilding);
	if (OwnerController)
	{
		OwnerController->ReceiveMoveCompleted.AddDynamic(this, &UPalCommandExecutor_Architecture::FinishMove);
		if (OwnerController->MoveToActor(Command.pTarget) == false)
		{
			EndBuilding();
		}
	}
}

void UPalCommandExecutor_Architecture::Abort()
{
	if (bActionStart == false)
		return;

	bActionStart = false;
	if (TargetBuilding)
	{
		TargetBuilding->GetBuildingProgress()->StopBuilding();
		TargetBuilding->GetBuildingProgress()->onBuildingEnd.RemoveDynamic(this, &UPalCommandExecutor_Architecture::EndBuilding);
		TargetBuilding = nullptr;
	}
	if (OwnerPal)
	{
		OwnerPal->SetArchitectureVisibility(false);
	}
	if (OwnerController)
	{
		OwnerController->StopMovement();
		OwnerController->ReceiveMoveCompleted.RemoveDynamic(this, &UPalCommandExecutor_Architecture::FinishMove);
	}
}

void UPalCommandExecutor_Architecture::EndBuilding()
{
	Abort();
	EndCommand();
}


void UPalCommandExecutor_Architecture::FinishMove(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (!TargetBuilding || TargetBuilding->GetBuildingProgress()->IsBuildingEnd())// || Result != EPathFollowingResult::Type::Success)
	{
		EndBuilding();
		return;
	}
	TargetBuilding->GetBuildingProgress()->StartBuilding();
	if (OwnerPal)
	{
		OwnerPal->SetArchitectureVisibility(true);
	}
}