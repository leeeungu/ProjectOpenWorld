#include "Pal/CommandExecutor/PalCommandExecutor_Transport.h"
#include "Navigation/PathFollowingComponent.h"
#include "Creature/Character/BaseCreature.h"
#include "Pal/Controller/PalAIController.h"
#include "Pal/Component/PalCommandComponent.h"
#include "Pal/Interface/CommanderManageable.h"
#include "Item/Interface/TransportInterface.h"
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

bool UPalCommandExecutor_Transport::StartCommand(const FPalCommand& Command)
{
	if (!OwnerCommandComp )
	{
		UE_LOG(LogTemp, Warning, TEXT("Executor_Transport :: No PalCommandComponent"));
		EndTransport();
		return false;
	}
	if (!OwnerPal || !OwnerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Executor_Transport :: Owner is not Pal %s"), *OwnerCommandComp->GetOwner()->GetFName().ToString());
		EndTransport();
		return false;
	}
	if (eTransportState != TransportState::None)
	{
		UE_LOG(LogTemp, Warning, TEXT("Executor_Transport :: Already Move"));
		EndTransport();
		return false;
	}
	if (!Command.pTarget.IsValid() || !Command.pTarget->Implements<UTransportInterface>())
	{
		UE_LOG(LogTemp, Warning, TEXT("Executor_Transport :: UnValidTarget"));
		EndTransport();
		return false;
	}

	if (OwnerController)
	{
		OwnerController->SetBBTargetLocation(Command.TargetLocation);
		IsCommandStarted = true;
		return true;
	}
	EndTransport();
	return false;
}

void UPalCommandExecutor_Transport::Abort()
{
	if (OwnerCommandComp && OwnerPal)
	{
		const FPalCommand* Command = OwnerCommandComp->GetCurrentCommand_C();
		if (Command->pTarget.IsValid() && eTransportState != TransportState::None)
		{
			ITransportInterface::Execute_OnTransportUnRegister(Command->pTarget.Get(), OwnerPal);
		}
	}
	eTransportState = TransportState::None;
	if (OwnerPal)
	{
		OwnerPal->SetActionStarted(false);
		OwnerPal->GetCharacterMovement()->MaxWalkSpeed = OwnerPal->GetDefaultSpeed();
	}
	if (OwnerController)
	{
		OwnerController->ResetMove();
	}
}

void UPalCommandExecutor_Transport::EndTransport()
{
	Abort();
	EndCommand();
	
}

void UPalCommandExecutor_Transport::WorkCommand()
{
	const FPalCommand* Command = OwnerCommandComp->GetCurrentCommand_C();
	if (!Command->pTarget.IsValid() || !Command->pTarget->Implements<UCommanderManageable>() || ICommanderManageable::Execute_IsCommandFinished(Command->pTarget.Get()))
	{
		EndTransport();
	}
	else if(Command->pTarget.IsValid() && Command->pTarget.Get())
	{
		if (eTransportState == TransportState::Go)
		{
			if (ITransportInterface::Execute_GetTransportState(Command->pTarget.Get()) == ETransportState::NotTransport)
			{
				eTransportState = TransportState::Back;
				if (OwnerController->MoveToLocation(Command->pInstigatorActor->GetActorLocation(), 300.0f))
				{
					if (Command->pTarget.IsValid())
					{
						ITransportInterface::Execute_OnTransportRegister(Command->pTarget.Get(), OwnerPal);
						OwnerPal->SetActionStarted(true);
						OwnerPal->GetCharacterMovement()->MaxWalkSpeed = 75.0f;
					}
					else
					{
						EndTransport();
					}
				}
				else
				{
					if (Command->pTarget.IsValid())
					{
						ITransportInterface::Execute_OnTransportUnRegister(Command->pTarget.Get(), OwnerPal);
						EndTransport();
					}
				}
			}
			else
			{
				EndTransport();
			}
		}
		else if (eTransportState == TransportState::Back )
		{
			if (ITransportInterface::Execute_GetTransportState(Command->pTarget.Get()) == ETransportState::Transport)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Transport :: End"));
				///if (FVector::DistSquared(Command->pInstigatorActor.Get()->GetActorLocation(), OwnerPal->GetActorLocation()) <= 90.0 * 90.0)
				{
					ITransportInterface::Execute_OnTransportUnRegister(Command->pTarget.Get(), OwnerPal);
					ITransportInterface::Execute_OnTransportEnd(Command->pTarget.Get(), OwnerPal);
					EndTransport();
				}
			}
			else
			{
				ITransportInterface::Execute_OnTransportUnRegister(Command->pTarget.Get(), OwnerPal);
				EndTransport();

			}
			//else
			//{
			//	//OwnerController->MoveToLocation(Command->pInstigatorActor->GetActorLocation(), 50.0f);
			//	ITransportInterface::Execute_OnTransportUnRegister(Command->pTarget.Get(), OwnerPal);
			//	EndTransport();
			//}
		}
	}
}

 bool UPalCommandExecutor_Transport::CheckCommandValid()
 {
	 if (!OwnerCommandComp || !OwnerPal || eTransportState == TransportState::None)
		 return false;
	 const FPalCommand* Command = OwnerCommandComp->GetCurrentCommand_C();
	 if (!OwnerCommandComp->IsValidCommand() || Command->CommandKind != EPalCommandKind::Work || Command->SubCommandType != (uint8)ESubWorkType::Transport ||
		 !Command->pInstigatorActor.IsValid() || !Command->pTarget.IsValid() || !Command->pTarget->Implements<UTransportInterface>())
		 
	 {
		 UE_LOG(LogTemp, Warning, TEXT("Executor_Transport :: not slef command"));
		 return false;
	 }
	 
	 return true;
 }