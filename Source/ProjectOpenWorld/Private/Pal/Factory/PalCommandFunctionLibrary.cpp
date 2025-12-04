#include "Pal/Factory/PalCommandFunctionLibrary.h"

FPalCommand UPalCommandFunctionLibrary::CommandMoveToLocation(AActor* pInstigator, FVector TargetLocation)
{
	FPalCommand command{};
	command.CommandKind = EPalCommandKind::Move;
	command.SubCommandType = (uint8)ESubMoveType::Location;
	command.TargetLocation = TargetLocation;
	command.pInstigatorActor = pInstigator;
	return command;
}

FPalCommand UPalCommandFunctionLibrary::CommandMoveToActor(AActor* pInstigator, AActor* pTargetActor)
{
	if (!pTargetActor)
		return FPalCommand();
	FPalCommand command{};
	command.CommandKind = EPalCommandKind::Move;
	command.SubCommandType = (uint8)ESubMoveType::Actor;
	command.pTarget = pTargetActor;
	command.pInstigatorActor = pInstigator;
	return command;
}

FPalCommand UPalCommandFunctionLibrary::CommandArchitecture(AActor* pInstigator, AActor* pTargetActor)
{
	if (!pTargetActor)
		return FPalCommand();
	FPalCommand command{};
	command.CommandKind = EPalCommandKind::Work;
	command.SubCommandType = (uint8)ESubWorkType::Architecture;
	command.pTarget = pTargetActor;
	command.pInstigatorActor = pInstigator;
	return command;
}

FPalCommand UPalCommandFunctionLibrary::CommandTransport(AActor* pInstigator, AActor* pTargetActor, AActor* pDestination)
{
	if (!pTargetActor || !pDestination || !pInstigator)
		return FPalCommand();
	FPalCommand command{};
	command.CommandKind = EPalCommandKind::Work;
	command.SubCommandType = (uint8)ESubWorkType::Transport;
	command.pTarget = pTargetActor;
	command.pInstigatorActor = pDestination;
	return command;
}

FPalCommand UPalCommandFunctionLibrary::CommandMining(AActor* pInstigator, AActor* pTargetActor)
{
	if (!pTargetActor)
		return FPalCommand();
	FPalCommand command{};
	command.CommandKind = EPalCommandKind::Work;
	command.SubCommandType = (uint8)ESubWorkType::Mining;
	command.pTarget = pTargetActor;
	command.pInstigatorActor = pInstigator;
	return command;
}

FPalCommand UPalCommandFunctionLibrary::CommandAttack(AActor* pInstigator, AActor* pTargetActor, ESubAttackType SlotNum)
{
	if (!pTargetActor)
		return FPalCommand();
	FPalCommand command{};
	command.CommandKind = EPalCommandKind::Attack;
	command.SubCommandType = (uint8)SlotNum;
	command.pTarget = pTargetActor;
	command.pInstigatorActor = pInstigator;
	return command;

}