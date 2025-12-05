#include "Pal/CommandExecutor/PalCommandExecutor_Transport.h"
#include "Navigation/PathFollowingComponent.h"
#include "Creature/Character/BaseCreature.h"
#include "Pal/Controller/PalAIController.h"
#include "Pal/Component/PalCommandComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UPalCommandExecutor_Transport::Initialize(UPalCommandComponent* CommandComp)
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

void UPalCommandExecutor_Transport::StartCommand(const FPalCommand& Command)
{
	if (!OwnerCommandComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Executor_Transport :: No PalCommandComponent"));
		return;
	}
	if (!OwnerPal || !OwnerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Executor_Transport :: Owner is not Pal %s"), *OwnerCommandComp->GetOwner()->GetFName().ToString());
		return;
	}
	if (eTransportState != TransportState::None)
	{
		UE_LOG(LogTemp, Warning, TEXT("Executor_Transport :: Already Move"));
		return;
	}
	if (!Command.pTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("Executor_Transport :: Can't move"));
		return;
	}
	if (OwnerController)
	{
		OwnerController->ReceiveMoveCompleted.AddDynamic(this, &UPalCommandExecutor_Transport::FinishMove);
		eTransportState = TransportState::Go;
		if (OwnerController->MoveToLocation(Command.pTarget->GetActorLocation(), 40.0f) == false)
		{
			EndTransport();
			return;
		}
	}
}

void UPalCommandExecutor_Transport::Abort()
{
	eTransportState = TransportState::None;
	if (OwnerPal)
	{
		OwnerPal->SetActionStarted(false);
		OwnerPal->GetCharacterMovement()->MaxWalkSpeed = OwnerPal->GetDefaultSpeed();
	}
	if (OwnerController)
	{
		OwnerController->StopMovement();
		OwnerController->ReceiveMoveCompleted.RemoveDynamic(this, &UPalCommandExecutor_Transport::FinishMove);
	}
}

void UPalCommandExecutor_Transport::EndTransport()
{
	Abort();
	EndCommand();
}

void UPalCommandExecutor_Transport::FinishMove(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if ( OwnerCommandComp != nullptr && OwnerController != nullptr)// || Result != EPathFollowingResult::Type::Success)
	{
		const FPalCommand* Command = OwnerCommandComp->GetCurrentCommand_C();
		if (!OwnerCommandComp->IsValidCommand() || Command->CommandKind != EPalCommandKind::Work || Command->SubCommandType != (uint8)ESubWorkType::Transport || !Command->pInstigatorActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("Executor__MoveLocation :: not slef command"));
			return;
		}
		else if (Result == EPathFollowingResult::Type::Success)
		{
			if (Command->pTarget && eTransportState == TransportState::Go)
			{
				eTransportState = TransportState::Back;
				if (OwnerController->MoveToLocation(Command->pInstigatorActor->GetActorLocation(), 40.0f) == false)
				{
					EndTransport();
				}
				else if (OwnerPal)
				{
					OwnerPal->SetActionStarted(true);
					OwnerPal->GetCharacterMovement()->MaxWalkSpeed =50.0f;
				}
				return;
			}
			else if (eTransportState == TransportState::Back)
			{
				// 운송 완료
			}
		}
	}
	EndTransport();
}