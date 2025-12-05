#include "Pal/CommandExecutor/PalCommandExecutor_Architecture.h"
#include "Building/BaseBuilding.h"
#include "Building/Component/BuildingProgress.h"
#include "Creature/Character/BaseCreature.h"
#include "Pal/Controller/PalAIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Pal/Component/PalCommandComponent.h"

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
	TargetBuilding = Cast< ABaseBuilding>(Command.pTarget);
	if (!TargetBuilding || TargetBuilding->GetBuildingProgress()->IsBuildingEnd() || !OwnerPal)
		return;
	
	UE_LOG(LogTemp, Warning, TEXT("Start Architecture :: %s"), *OwnerCommandComp->GetOwner()->GetFName().ToString());
	bActionStart = true;
	TargetBuilding->GetBuildingProgress()->onBuildingEnd.AddDynamic(this, &UPalCommandExecutor_Architecture::EndBuilding);
	if (OwnerController)
	{
		OwnerController->ReceiveMoveCompleted.AddDynamic(this, &UPalCommandExecutor_Architecture::FinishMove);
		if (OwnerController->MoveToActor(Command.pTarget, 200.f) == false)
		{
			EndBuilding();
		}
	}
}

void UPalCommandExecutor_Architecture::Abort()
{
	if (bActionStart == false)
		return;
	UE_LOG(LogTemp, Warning, TEXT("Executor_Architecture :: Abort"));
	bActionStart = false;
	if (TargetBuilding)
	{
		TargetBuilding->GetBuildingProgress()->StopBuilding();
		TargetBuilding->GetBuildingProgress()->onBuildingEnd.RemoveDynamic(this, &UPalCommandExecutor_Architecture::EndBuilding);
		TargetBuilding = nullptr;
	}
	if (OwnerPal)
	{
		OwnerPal->SetActionStarted(false);
		if (UStaticMeshComponent* ArchitectureMeshComponent = OwnerPal->GetArchitectureMeshComponent())
		{
			ArchitectureMeshComponent->SetVisibility(false);
		}
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
	const FPalCommand* Command = OwnerCommandComp->GetCurrentCommand_C();
	if (!OwnerCommandComp->IsValidCommand() || Command->CommandKind != EPalCommandKind::Work || Command->SubCommandType != (uint8)ESubWorkType::Architecture)
	{
		UE_LOG(LogTemp, Warning, TEXT("Executor_Architecture :: not slef command"));
		return;
	}

	if (!TargetBuilding || TargetBuilding->GetBuildingProgress()->IsBuildingEnd())// || Result != EPathFollowingResult::Type::Success)
	{
		EndBuilding();
		return;
	}
	if (bActionStart)
	{
		TargetBuilding->GetBuildingProgress()->StartBuilding();
		if (OwnerPal)
		{
			OwnerPal->SetActionStarted(true);
			if (UStaticMeshComponent* ArchitectureMeshComponent = OwnerPal->GetArchitectureMeshComponent())
			{
				ArchitectureMeshComponent->SetVisibility(true);
			}
		}
	}
}