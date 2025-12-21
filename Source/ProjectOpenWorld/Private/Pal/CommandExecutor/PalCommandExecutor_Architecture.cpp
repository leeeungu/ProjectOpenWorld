#include "Pal/CommandExecutor/PalCommandExecutor_Architecture.h"
#include "Building/BaseBuilding.h"
#include "Building/Component/BuildingProgress.h"
#include "Creature/Character/BaseCreature.h"
#include "Pal/Controller/PalAIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Pal/Component/PalCommandComponent.h"
#include "Engine/StaticMeshSocket.h"

DEFINE_LOG_CATEGORY(ArchitectureCommand);


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

bool UPalCommandExecutor_Architecture::StartCommand(const FPalCommand& Command)
{
	TargetBuilding = Cast< ABaseBuilding>(Command.pTarget);
	if (!TargetBuilding || TargetBuilding->GetBuildingProgress()->IsBuildingEnd() || !OwnerPal)
		return false;
	
	UE_LOG(ArchitectureCommand, Warning, TEXT("StartCommand %s"), *OwnerPal->GetFName().ToString());
	bActionStart = true;
	IsCommandStarted = true;
	TargetBuilding->GetBuildingProgress()->onBuildingEnd.AddUniqueDynamic(this, &UPalCommandExecutor_Architecture::EndBuilding);
	if (OwnerController)
	{
		OwnerController->ReceiveMoveCompleted.AddUniqueDynamic(this, &UPalCommandExecutor_Architecture::FinishMove);
		FVector Target = TargetBuilding->GetBuildingMeshComponent()->GetSocketLocation(TEXT("Bottom"));
		EPathFollowingRequestResult::Type PathResult = OwnerController->MoveToLocation(Target, 600.f);
		if (PathResult == EPathFollowingRequestResult::Type::Failed)
		{
			UE_LOG(ArchitectureCommand, Error, TEXT("StartCommand::Can Find Path %s ") , *TargetBuilding->GetFullName());
			IsCommandStarted = false;
			EndBuilding();
			return false;
		}
		if (PathResult == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			if (OwnerCommandComp->IsValidCommand() &&
				Command.CommandKind == EPalCommandKind::Work && Command.SubCommandType == (uint8)ESubWorkType::Architecture)
			{
				TargetBuilding->GetBuildingProgress()->StartBuilding();
				if (OwnerPal) // aniation
				{
					OwnerPal->SetActionStarted(true);
					if (UStaticMeshComponent* ArchitectureMeshComponent = OwnerPal->GetArchitectureMeshComponent())
					{
						ArchitectureMeshComponent->SetVisibility(true);
					}
				}
			}
		}
		return true;
	}
	return false;
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
	UE_LOG(ArchitectureCommand, Warning, TEXT("Executor_Architecture :: EndBuilding"));
	Abort();
	EndCommand();
}


void UPalCommandExecutor_Architecture::FinishMove(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	const FPalCommand* Command = OwnerCommandComp->GetCurrentCommand_C();
	switch (Result)
	{
	case EPathFollowingResult::Success:
		UE_LOG(LogTemp, Warning, TEXT("Executor_Architecture :: FinishMove Success"));
		break;
	case EPathFollowingResult::Blocked:
		UE_LOG(LogTemp, Warning, TEXT("Executor_Architecture :: FinishMove Blocked"));
		break;
	case EPathFollowingResult::OffPath:
		UE_LOG(LogTemp, Warning, TEXT("Executor_Architecture :: FinishMove OffPath"));
		break;
	case EPathFollowingResult::Aborted:
		UE_LOG(LogTemp, Warning, TEXT("Executor_Architecture :: FinishMove Aborted"));
		break;
	case EPathFollowingResult::Skipped_DEPRECATED:
		UE_LOG(LogTemp, Warning, TEXT("Executor_Architecture :: FinishMove Skipped"));
		break;
	case EPathFollowingResult::Invalid:
		UE_LOG(LogTemp, Warning, TEXT("Executor_Architecture :: FinishMove Invalid"));
		return;
		break;
	default:
		break;
	}
	if (!bActionStart)
	{
		Abort();
		return;
	}

	if (!TargetBuilding || TargetBuilding->GetBuildingProgress()->IsBuildingEnd())// || Result != EPathFollowingResult::Type::Success)
	{
		EndBuilding();
		return;
	}

	if ( OwnerCommandComp->IsValidCommand() &&
		Command->CommandKind == EPalCommandKind::Work && Command->SubCommandType == (uint8)ESubWorkType::Architecture)
	{
		TargetBuilding->GetBuildingProgress()->StartBuilding();
		if (OwnerPal) // aniation
		{
			OwnerPal->SetActionStarted(true);
			if (UStaticMeshComponent* ArchitectureMeshComponent = OwnerPal->GetArchitectureMeshComponent())
			{
				ArchitectureMeshComponent->SetVisibility(true);
			}
		}
		return;
	}
	EndBuilding();
}