#include "Pal/Component/PalCommandComponent.h"

UPalCommandComponent::UPalCommandComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetPushCommandFunc(&UPalCommandComponent::PushCommand);
}

void UPalCommandComponent::ResetCommand(FPalCommand& pData)
{
	pData.CommandKind = EPalCommandKind::None;
	pData.pTarget = nullptr;
	pData.pInstigatorActor = nullptr;
}

void UPalCommandComponent::ResetCurrentCommand()
{
	ResetCommand(DummyCommand);
	CurrentCommand = &DummyCommand;
}

void UPalCommandComponent::BeginPlay()
{
	CommandPool.Init(FPalCommand{}, CommandSize::MaxSize);
	QueueEmpty.Empty();
	for (int i = 0; i < CommandSize::MaxSize; i++)
	{
		QueueEmpty.Enqueue(&CommandPool[i]);
	}
	QueueCommand.Empty();
	ResetCurrentCommand();
	UActorComponent::BeginPlay();
}

void UPalCommandComponent::PushCommand(const FPalCommand& NewCommand)
{
	FPalCommand* pEmpthy{};
	if (QueueEmpty.Dequeue(pEmpthy) == false)
	{
		UE_LOG(LogTemp, Log, TEXT("%s : Command Full"), *GetOwner()->GetName());
		return;
	}
	pEmpthy->CommandKind = NewCommand.CommandKind;
	pEmpthy->pInstigatorActor = NewCommand.pInstigatorActor;
	pEmpthy->pTarget = NewCommand.pTarget;
	QueueCommand.Enqueue(pEmpthy);
	PopCommand();
}

void UPalCommandComponent::PopCommand()
{
	if (CurrentCommand != &DummyCommand)
		return;

	if (QueueCommand.Dequeue(CurrentCommand) == false)
	{
		ResetCommand(*CurrentCommand);
		ResetCurrentCommand();
		return;
	}
	OnStartCurrentCommand();
}

void UPalCommandComponent::PushCommand_DequqOld(const FPalCommand& NewCommand)
{
	FPalCommand* pEmpthy{};
	if (QueueEmpty.Dequeue(pEmpthy) == false)
	{
		if (QueueCommand.Dequeue(pEmpthy) == false)
			return;
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

void UPalCommandComponent::PushCommand_Attack(AActor* pInstigatorActor, AActor* pTarget)
{
	FPalCommand Data{};
	Data.pInstigatorActor = pInstigatorActor;
	Data.CommandKind = EPalCommandKind::Attack;
	Data.pTarget = pTarget;
	if (!PushCommandFunc)
	{
		UE_LOG(LogTemp, Error, TEXT("%s : PushCommandFunc is null"), *GetName());
		return;
	}
	(this->*PushCommandFunc)(Data);
}

void UPalCommandComponent::FinishCommand()
{
	if (CurrentCommand != &DummyCommand)
	{
		ResetCommand(*CurrentCommand);
		QueueEmpty.Enqueue(CurrentCommand);
		OnEndCurrentCommand();
		ResetCurrentCommand();
	}
	PopCommand();
}
