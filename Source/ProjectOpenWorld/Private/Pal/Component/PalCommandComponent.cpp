#include "Pal/Component/PalCommandComponent.h"

UPalCommandComponent::UPalCommandComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetPushCommandFunc(&UPalCommandComponent::PushCommand_Default);
	CommandPool.Init(FPalCommand{}, CommandSize::MaxSize);
	QueueEmpty.Empty();
	for (int i = 0; i < CommandSize::MaxSize; i++)
	{
		QueueEmpty.Enqueue(&CommandPool[i]);
	}
	QueueCommand.Empty();
	ResetCurrentCommand();
}

void UPalCommandComponent::ResetCommand(FPalCommand& pData)
{
	pData.CommandKind = EPalCommandKind::None_PalCommandKind;
	pData.pTarget = nullptr;
	pData.pInstigatorActor = nullptr;
}

void UPalCommandComponent::ResetCurrentCommand()
{
	ResetCommand(DummyCommand);
	CurrentCommand = &DummyCommand;
	CurrentExcute = nullptr;
}

void UPalCommandComponent::BeginPlay()
{
	UActorComponent::BeginPlay();
}


void UPalCommandComponent::PopCommand()
{
	if (CurrentCommand != &DummyCommand)
		return;

	if (QueueCommand.Dequeue(CurrentCommand) == false)
	{
		ResetCommand(*CurrentCommand);
		ResetCurrentCommand();
		LastCommand = nullptr;
		//UE_LOG(LogTemp, Log, TEXT("%s : Command None"), *GetOwner()->GetName());
		return;
	}
	if (IsValidCommand() == false)
		return;
	const FPalCommand* Current = GetCurrentCommand_C();
	uint8 idx = (uint8)Current->CommandKind;
	if (!CommandExecutors.IsValidIndex(idx) || !CommandExecutors[idx].IsValidIndex(Current->SubCommandType))
		return;
	if (CurrentExcute == CommandExecutors[(uint8)Current->CommandKind][Current->SubCommandType].Get())
		return;

	CurrentExcute = CommandExecutors[(uint8)Current->CommandKind][Current->SubCommandType].Get();
	if (CurrentExcute)
	{
		CurrentExcute->StartCommand(*Current);
	}
	OnStartCurrentCommand();
}

void UPalCommandComponent::PushCommand_Default(const FPalCommand& NewCommand)
{
	FPalCommand* pEmpthy{};
	if (LastCommand)
	{
		if (NewCommand == *LastCommand)
		{
			UE_LOG(LogTemp, Log, TEXT("%s : Duple Command"), *GetOwner()->GetName());
			return;
		}
	}
	if (QueueEmpty.Dequeue(pEmpthy) == false)
	{
		UE_LOG(LogTemp, Log, TEXT("%s : Command Full"), *GetOwner()->GetName());
		PopCommand();
		return;
	}
	*pEmpthy = NewCommand;
	LastCommand = pEmpthy;
	QueueCommand.Enqueue(pEmpthy);
	PopCommand();
}

void UPalCommandComponent::PushCommand_DequqOld(const FPalCommand& NewCommand)
{
	FPalCommand* pEmpthy{};
	if (QueueEmpty.Dequeue(pEmpthy) == false)
	{
		if (QueueCommand.Dequeue(pEmpthy) == false)
			return;
		PopCommand();
	}
	pEmpthy->CommandKind = NewCommand.CommandKind;
	pEmpthy->pInstigatorActor = NewCommand.pInstigatorActor;
	pEmpthy->pTarget = NewCommand.pTarget;
	QueueCommand.Enqueue(pEmpthy);
	PopCommand();
}

void UPalCommandComponent::SetPushCommandFunc(void(UPalCommandComponent::* Func)(const FPalCommand&))
{
	PushCommandFunc = Func;
}

void UPalCommandComponent::PushCommand(const FPalCommand& NewCommand)
{
	if (!PushCommandFunc)
	{
		UE_LOG(LogTemp, Error, TEXT("%s : PushCommandFunc is null"), *GetName());
		return;
	}
	(this->*PushCommandFunc)(NewCommand);
}

void UPalCommandComponent::FinishCommand()
{
	if (CurrentCommand != &DummyCommand)
	{
		if (CurrentExcute)
		{
			CurrentExcute->Abort();
		}
		if (LastCommand == CurrentCommand)
		{
			LastCommand = nullptr;
		}
		FPalCommand command = *CurrentCommand;
		QueueEmpty.Enqueue(CurrentCommand);
		ResetCurrentCommand();
		OnFinishedCurrentCommand();
		if (OnCommandFinished.IsBound())
		{
			OnCommandFinished.Broadcast(GetOwner(), command);
		}
	}
	PopCommand();
}

void UPalCommandComponent::ResetCommandQue()
{
	FinishCommand();
	QueueCommand.Empty();
	QueueEmpty.Empty();
	for (int i = 0; i < CommandSize::MaxSize; i++)
	{
		ResetCommand(CommandPool[i]);
		QueueEmpty.Enqueue(&CommandPool[i]);
	}
	LastCommand = nullptr;
	ResetCommand(DummyCommand);
	ResetCurrentCommand();
}
