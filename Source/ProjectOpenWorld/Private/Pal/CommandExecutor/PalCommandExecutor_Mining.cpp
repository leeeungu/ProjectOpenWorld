#include "Pal/CommandExecutor/PalCommandExecutor_Mining.h"
#include "Creature/Character/BaseCreature.h"
#include "Pal/Controller/PalAIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Pal/Component/PalCommandComponent.h"
#include "Interaction/Component/InteractionComponent.h"	

void UPalCommandExecutor_Mining::Initialize(UPalCommandComponent* CommandComp)
{
	UPalCommandExecutorBase::Initialize(CommandComp);
	if (OwnerCommandComp == nullptr)
		return;
	ABaseCreature* OwnerCreature = Cast<ABaseCreature>(OwnerCommandComp->GetOwner());
	if (!OwnerCreature)
		return;
	OwnerPal = OwnerCreature;
	OwnerController = Cast<APalAIController>(OwnerPal->GetController());
	InteractionComp = OwnerCreature->GetInteractionComponent();
}

bool UPalCommandExecutor_Mining::StartCommand(const FPalCommand& Command)
{
	if (!OwnerPal || !OwnerController || !Command.pTarget.IsValid())
		return false;

	if (OwnerController)
	{
		bStartedMining = true;
		//OwnerController->ReceiveMoveCompleted.AddUniqueDynamic(this, &UPalCommandExecutor_Mining::FinishMove);
		EPathFollowingRequestResult::Type PathResult = OwnerController->MoveToLocation(Command.pTarget->GetActorLocation(), 100.f);
		if (PathResult == EPathFollowingRequestResult::Type::Failed)
		{
			if (Command.pTarget.IsValid())
				UE_LOG(LogTemp, Warning, TEXT("Mining::Can Find Path %s "), *Command.pTarget->GetName());
			EndMining();
			return false;
		}
		else if (PathResult == EPathFollowingRequestResult::Type::AlreadyAtGoal)
		{
			IsCommandStarted = true;
			OwnerPal->SetActionStarted(true);
			InteractionComp->SetInteractionTarget(Command.pTarget.Get());
			InteractionComp->OnInteractionStart();
		}
	}
	IsCommandStarted = true;
	return true;
}

void UPalCommandExecutor_Mining::Abort()
{
	bStartedMining = false;
	if (OwnerPal)
	{
		OwnerPal->SetActionStarted(false);
	}
	if (OwnerController)
	{
		OwnerController->StopMovement();
		//OwnerController->ReceiveMoveCompleted.RemoveDynamic(this, &UPalCommandExecutor_Mining::FinishMove);
	}
}

void UPalCommandExecutor_Mining::WorkCommand()
{
	const FPalCommand* Command = OwnerCommandComp->GetCurrentCommand_C();
	OwnerPal->SetActionStarted(true);
	InteractionComp->SetInteractionTarget(Command->pTarget.Get());
	InteractionComp->OnInteractionStart();
}

bool UPalCommandExecutor_Mining::CheckCommandValid()
{
	if (!OwnerCommandComp || !OwnerPal || !bStartedMining)
		return false;

	const FPalCommand* Command = OwnerCommandComp->GetCurrentCommand_C();
	if (!OwnerCommandComp->IsValidCommand() || Command->CommandKind != EPalCommandKind::Work ||
		Command->SubCommandType != (uint8)ESubWorkType::Mining || !Command->pTarget.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Cant Mining"));
		return false;
	}

	return true;
}

void UPalCommandExecutor_Mining::EndMining()
{
	Abort();
	EndCommand();
}

//void UPalCommandExecutor_Mining::FinishMove(FAIRequestID RequestID, EPathFollowingResult::Type Result)
//{
//	if (!bStartedMining && !OwnerCommandComp)
//		return;
//	const FPalCommand* Command = OwnerCommandComp->GetCurrentCommand_C();
//	if (!OwnerCommandComp->IsValidCommand() || Command->CommandKind != EPalCommandKind::Work || Command->SubCommandType != (uint8)ESubWorkType::Mining|| !Command->pTarget.IsValid())
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Executor_MoveActor :: not slef command"));
//		return;
//	}
//	if (OwnerPal && FVector::Distance(Command->pTarget->GetActorLocation(), OwnerPal->GetActorLocation()) <= 300.0f)
//	{
//		OwnerPal->SetActionStarted(true);
//		InteractionComp->SetInteractionTarget(Command->pTarget.Get());
//		InteractionComp->OnInteractionStart();
//		return;
//	}
//	EndMining();
//}