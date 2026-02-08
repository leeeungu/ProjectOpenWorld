#include "Pal/CommandExecutor/PalCommandExecutor_Architecture.h"
#include "Building/BaseBuilding.h"
#include "Building/Component/PalBuildingStaticMeshComponent.h"
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
	{
		EndBuilding();
		return false;
	}
	if (OwnerController)
	{
		
		FVector Target = TargetBuilding->GetBuildingMeshComponent()->GetSocketLocation(TEXT("Bottom"));
		OwnerController->SetBBTargetLocation(Target);
		IsCommandStarted = true;
		return true;
	}
	EndBuilding();
	return false;
}

void UPalCommandExecutor_Architecture::Abort()
{
	if (bActionStart == false)
		return;
	bActionStart = false;
	IsCommandStarted = false;
	if (TargetBuilding)
	{
		TargetBuilding->GetBuildingProgress()->StopBuilding(OwnerPal);
		TargetBuilding = nullptr;
	}
	if (OwnerController)
	{
		OwnerController->ResetMove();
	}
}

void UPalCommandExecutor_Architecture::WorkCommand()
{
	if (!bActionStart)
	{
		bActionStart = true;
		TargetBuilding->GetBuildingProgress()->StartBuilding(OwnerPal);
	}
}

bool UPalCommandExecutor_Architecture::CheckCommandValid()
{
	if (!OwnerCommandComp)
		return false;

	const FPalCommand* Command = OwnerCommandComp->GetCurrentCommand_C();
	if (!OwnerCommandComp->IsValidCommand() || Command->CommandKind != EPalCommandKind::Work ||
		Command->SubCommandType != (uint8)ESubWorkType::Architecture || !Command->pTarget.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("UPalCommandExecutor_Architecture :: CheckCommandValid"));
		return false;
	}
	if (!TargetBuilding || TargetBuilding->GetBuildingProgress()->IsBuildingEnd())// || Result != EPathFollowingResult::Type::Success)
	{
		UE_LOG(LogTemp, Error, TEXT("UPalCommandExecutor_Architecture :: Can't build"));
		return false;
	}
	return true;
}

void UPalCommandExecutor_Architecture::EndBuilding()
{
	Abort();
	EndCommand();
}
//
//void UPalCommandExecutor_Architecture::FinishMove(FAIRequestID RequestID, EPathFollowingResult::Type Result)
//{
//	WorkCommand();
//	//const FPalCommand* Command = OwnerCommandComp->GetCurrentCommand_C();
//	//if (!OwnerCommandComp->IsValidCommand() || Command->CommandKind != EPalCommandKind::Work ||
//	//	Command->SubCommandType != (uint8)ESubWorkType::Architecture || !Command->pTarget.IsValid())
//	//{
//	//	return;
//	//}
//	///*switch (Result)
//	//{
//	//case EPathFollowingResult::Success:
//	//	UE_LOG(LogTemp, Warning, TEXT("Executor_Architecture :: FinishMove Success"));
//	//	break;
//	//case EPathFollowingResult::Blocked:
//	//	UE_LOG(LogTemp, Warning, TEXT("Executor_Architecture :: FinishMove Blocked"));
//	//	break;
//	//case EPathFollowingResult::OffPath:
//	//	UE_LOG(LogTemp, Warning, TEXT("Executor_Architecture :: FinishMove OffPath"));
//	//	break;
//	//case EPathFollowingResult::Aborted:
//	//	UE_LOG(LogTemp, Warning, TEXT("Executor_Architecture :: FinishMove Aborted"));
//	//	break;
//	//case EPathFollowingResult::Skipped_DEPRECATED:
//	//	UE_LOG(LogTemp, Warning, TEXT("Executor_Architecture :: FinishMove Skipped"));
//	//	break;
//	//case EPathFollowingResult::Invalid:
//	//	UE_LOG(LogTemp, Warning, TEXT("Executor_Architecture :: FinishMove Invalid"));
//	//	return;
//	//	break;
//	//default:
//	//	break;
//	//}*/
//	//if (!bActionStart)
//	//{
//	//	Abort();
//	//	return;
//	//}
//
//	//if (!TargetBuilding || TargetBuilding->GetBuildingProgress()->IsBuildingEnd())// || Result != EPathFollowingResult::Type::Success)
//	//{
//	//	EndBuilding();
//	//	return;
//	//}
//
//	//if ( OwnerCommandComp->IsValidCommand() &&
//	//	Command->CommandKind == EPalCommandKind::Work && Command->SubCommandType == (uint8)ESubWorkType::Architecture)
//	//{
//	//	TargetBuilding->GetBuildingProgress()->StartBuilding(OwnerPal);
//	//	return;
//	//}
//	//EndBuilding();
//}