#include "Pal/Component/PalCommandComponent.h"
#include "Pal/Controller/PalAIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/Character.h"

UPalCommandComponent::UPalCommandComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetPushCommandFunc(&UPalCommandComponent::PushCommand_Default);
	ResetCurrentCommand();
	SetComponentTickEnabled(false);
}

void UPalCommandComponent::ResetCommand(FPalCommand& pData)
{
	pData.Reset();
}

void UPalCommandComponent::ResetCurrentCommand()
{
	ResetCommand(DummyCommand);
	if (CurrentCommand)
		ResetCommand(*CurrentCommand);
	CurrentCommand = &DummyCommand;
	CurrentCommandData = *CurrentCommand;
	CurrentExcute = nullptr;
}

void UPalCommandComponent::BeginPlay()
{
	UActorComponent::BeginPlay();
	QueueEmpty.Empty();
	QueueCommand.Empty();
	CommandPool.Init(FPalCommand{}, CommandSize::MaxSize);
	for (int i = 0; i < CommandSize::MaxSize; i++)
	{
		QueueEmpty.Enqueue(&CommandPool[i]);
	}
	ACharacter* OwnerPal = Cast<ACharacter>(GetOwner());
	if (OwnerPal)
	{
		APalAIController* OwnerController = Cast<APalAIController>(OwnerPal->GetController());
		if (OwnerController)
		{
			OwnerController->ReceiveMoveCompleted.AddUniqueDynamic(this, &UPalCommandComponent::FinishMove);
		}
	}
}

bool UPalCommandComponent::PopCommand()
{
	if (CurrentCommand != &DummyCommand)
		return false;

	if (QueueCommand.Dequeue(CurrentCommand) == false)
	{
		SetComponentTickEnabled(false);
		ResetCommand(*CurrentCommand);
		ResetCurrentCommand();
		LastCommand = nullptr;
		//UE_LOG(LogTemp, Log, TEXT("%s : Command None"), *GetOwner()->GetName());
		return false;
	}
	if (IsValidCommand() == false)
		return false;

	const FPalCommand* Current = GetCurrentCommand_C();
	uint8 idx = (uint8)Current->CommandKind;
	if (!CommandExecutors.IsValidIndex(idx) || !CommandExecutors[idx].IsValidIndex(Current->SubCommandType))
		return false;
	if (CurrentExcute == CommandExecutors[(uint8)Current->CommandKind][Current->SubCommandType].Get())
		return false;

	CurrentExcute = CommandExecutors[(uint8)Current->CommandKind][Current->SubCommandType].Get();
	if (CurrentExcute)
	{
		if (CurrentExcute->StartCommand(*Current))
		{
			CurrentCommandData = *CurrentCommand;
			OnStartCurrentCommand();
			return true;	
		}
		else
		{
			FinishCommand();
			return true;
		}
	}
	return false;
}

bool UPalCommandComponent::PushCommand_Default(const FPalCommand& NewCommand)
{
	FPalCommand* pEmpthy{};
	if (LastCommand)
	{
		if (NewCommand == *LastCommand)
		{
			//UE_LOG(LogTemp, Log, TEXT("%s : Duple Command"), *GetOwner()->GetName());
			return true;
		}
	}
	if (QueueEmpty.Dequeue(pEmpthy) == false)
	{
		//UE_LOG(LogTemp, Log, TEXT("%s : Command Full"), *GetOwner()->GetName());
		PopCommand();
		return false;
	}
	*pEmpthy = NewCommand;
	LastCommand = pEmpthy;
	QueueCommand.Enqueue(pEmpthy);
	SetComponentTickEnabled(true);
	return PopCommand();
}

bool UPalCommandComponent::PushCommand_DequqOld(const FPalCommand& NewCommand) // ¹Ì»ç¿ë
{
	FPalCommand* pEmpthy{};
	if (QueueEmpty.Dequeue(pEmpthy) == false)
	{
		if (QueueCommand.Dequeue(pEmpthy) == false)
			return false;
	
		return 	PopCommand();
	}
	pEmpthy->CommandKind = NewCommand.CommandKind;
	pEmpthy->pInstigatorActor = NewCommand.pInstigatorActor;
	pEmpthy->pTarget = NewCommand.pTarget;
	QueueCommand.Enqueue(pEmpthy);
	SetComponentTickEnabled(true);
	return PopCommand();
}

void UPalCommandComponent::SetPushCommandFunc(bool(UPalCommandComponent::* Func)(const FPalCommand&))
{
	PushCommandFunc = Func;
}

void UPalCommandComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!QueueCommand.IsEmpty() && !IsValidCommand())
	{
		PopCommand();
	}
}

bool UPalCommandComponent::PushCommand(const FPalCommand& NewCommand)
{
	if (!PushCommandFunc)
	{
		UE_LOG(LogTemp, Error, TEXT("%s : PushCommandFunc is null"), *GetName());
		return false;
	}
	return (this->*PushCommandFunc)(NewCommand);
}

void UPalCommandComponent::FinishCommand()
{
	if (CurrentCommand != &DummyCommand)
	{
		if (CurrentExcute)
			CurrentExcute->Abort();
		if (CurrentExcute)
			CurrentExcute->ResetStarted();
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
	if (CurrentExcute)
	{
		CurrentExcute->Abort();
		CurrentExcute->ResetStarted();
	}
	ResetCurrentCommand();
}

void UPalCommandComponent::FinishMove(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (!CurrentExcute)// || Result == EPathFollowingResult::Type::Invalid || Result == EPathFollowingResult::Type::OffPath)
	{
		return;
	}
	if (CurrentExcute->CheckCommandValid())
	{
		//if (Result == EPathFollowingResult::Type::Success)
			CurrentExcute->WorkCommand();
		//else if (Result == EPathFollowingResult::Type::Aborted)// || Result == EPathFollowingResult::Type::Blocked)
		//{
		//	CurrentExcute->Abort();
		//	CurrentExcute->StartCommand(*CurrentCommand);
		//}
	}
	else
	{
		FinishCommand();
	}
}