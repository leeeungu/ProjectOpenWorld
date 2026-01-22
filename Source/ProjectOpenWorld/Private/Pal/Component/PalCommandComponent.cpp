#include "Pal/Component/PalCommandComponent.h"
#include "Pal/Controller/PalAIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/Character.h"

UPalCommandComponent_Pre::UPalCommandComponent_Pre()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetPushCommandFunc(&UPalCommandComponent_Pre::PushCommand_Default);
	ResetCurrentCommand();
	SetComponentTickEnabled(false);
}

void UPalCommandComponent_Pre::ResetCommand(FPalCommand& pData)
{
	pData.Reset();
}

void UPalCommandComponent_Pre::ResetCurrentCommand()
{
	ResetCommand(DummyCommand);
	if (CurrentCommand)
		ResetCommand(*CurrentCommand);
	CurrentCommand = &DummyCommand;
	CurrentCommandData = *CurrentCommand;
	CurrentExcute = nullptr;
}

void UPalCommandComponent_Pre::BeginPlay()
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
			OwnerController->ReceiveMoveCompleted.AddUniqueDynamic(this, &UPalCommandComponent_Pre::FinishMove);
		}
	}
}

bool UPalCommandComponent_Pre::PopCommand()
{
	if (CurrentCommand != &DummyCommand)
	{
		UE_LOG(LogTemp, Error, TEXT("%s : Current Command Not Finished"), *GetOwner()->GetName());
		return false;
	}

	if (QueueCommand.Dequeue(CurrentCommand) == false)
	{
		SetComponentTickEnabled(false);
		ResetCommand(*CurrentCommand);
		ResetCurrentCommand();	
		LastCommand = nullptr;
		UE_LOG(LogTemp, Log, TEXT("%s : Command None"), *GetOwner()->GetName());
		return false;
	}
	if (IsValidCommand() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("%s : Command Invalid"), *GetOwner()->GetName());
		return false;
	}

	const FPalCommand* Current = GetCurrentCommand_C();
	uint8 idx = (uint8)Current->CommandKind;
	if (!CommandExecutors.IsValidIndex(idx) || !CommandExecutors[idx].IsValidIndex(Current->SubCommandType))
	{
		UE_LOG(LogTemp, Error, TEXT("%s : Command Executor Not Found"), *GetOwner()->GetName());
		return false;
	}
	if (CurrentExcute == CommandExecutors[(uint8)Current->CommandKind][Current->SubCommandType].Get())
	{
		UE_LOG(LogTemp, Error, TEXT("%s : Same Command Executor Running"), *GetOwner()->GetName());
		return false;
	}

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
	UE_LOG(LogTemp, Error, TEXT("%s : Command Executor is null"), *GetOwner()->GetName());
	return false;
}

bool UPalCommandComponent_Pre::PushCommand_Default(const FPalCommand& NewCommand)
{
	FPalCommand* pEmpthy{};
	if (LastCommand)
	{
		if (NewCommand == *LastCommand)
		{
			UE_LOG(LogTemp, Log, TEXT("%s : Duple Command"), *GetOwner()->GetName());
			return true;
		}
	}
	if (QueueEmpty.Dequeue(pEmpthy) == false)
	{
		UE_LOG(LogTemp, Log, TEXT("%s : Command Full"), *GetOwner()->GetName());
		PopCommand();
		return false;
	}
	*pEmpthy = NewCommand;
	LastCommand = pEmpthy;
	QueueCommand.Enqueue(pEmpthy);
	SetComponentTickEnabled(true);
	return PopCommand();
}

bool UPalCommandComponent_Pre::PushCommand_DequqOld(const FPalCommand& NewCommand) // 미사용
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

void UPalCommandComponent_Pre::SetPushCommandFunc(bool(UPalCommandComponent_Pre::* Func)(const FPalCommand&))
{
	PushCommandFunc = Func;
}

void UPalCommandComponent_Pre::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!QueueCommand.IsEmpty() && !IsValidCommand())
	{
		PopCommand();
	}
}

bool UPalCommandComponent_Pre::PushCommand(const FPalCommand& NewCommand)
{
	if (!PushCommandFunc)
	{
		UE_LOG(LogTemp, Error, TEXT("%s : PushCommandFunc is null"), *GetName());
		return false;
	}
	return (this->*PushCommandFunc)(NewCommand);
}

void UPalCommandComponent_Pre::FinishCommand()
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
		if (CurrentCommand != &DummyCommand)
			UE_LOG(LogTemp, Error, TEXT("%s : Current Command Not Reset After Finish"), *GetOwner()->GetName());
		if (OnCommandFinished.IsBound())
		{
			OnCommandFinished.Broadcast(GetOwner(), command);
		}
	}
	PopCommand();
}

void UPalCommandComponent_Pre::ResetCommandQue()
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
	UE_LOG(LogTemp, Log, TEXT("%s : Command Queue Reset"), *GetOwner()->GetName());
}

void UPalCommandComponent_Pre::FinishMove(FAIRequestID RequestID, EPathFollowingResult::Type Result)
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

bool UPalCommandComponent::PushCommand_Default(const FPalCommand& NewCommand)
{
	CurrentCommand = &CurrentCommandData;
	//if (CurrentCommandData == NewCommand)
	//	UE_LOG(LogTemp, Error, TEXT("%s : PushCommand_Default Equal"), *GetOwner()->GetName());
	//if (FPalCommand::IsValidCommand(CurrentCommandData))
	//	UE_LOG(LogTemp, Error, TEXT("%s : PushCommand_Default IsValidCommand"), *GetOwner()->GetName());
	if (CurrentCommandData == NewCommand || FPalCommand::IsValidCommand(CurrentCommandData))
		return false;
	const FPalCommand* Current = &NewCommand;
	uint8 idx = (uint8)Current->CommandKind;

	if (!CommandExecutors.IsValidIndex(idx) || !CommandExecutors[idx].IsValidIndex(Current->SubCommandType))
	{
		UE_LOG(LogTemp, Error, TEXT("%s : Command Executor Not Found"), *GetOwner()->GetName());
		return false;
	}
	/*if (CurrentExcute == CommandExecutors[(uint8)Current->CommandKind][Current->SubCommandType].Get())
	{
		UE_LOG(LogTemp, Error, TEXT("%s : Same Command Executor Running"), *GetOwner()->GetName());
		return false;
	}*/

	
	CurrentExcute = CommandExecutors[(uint8)Current->CommandKind][Current->SubCommandType].Get();
	if (CurrentExcute)
	{
		CurrentCommandData = NewCommand;
		if (CurrentExcute->StartCommand(*Current))
		{
			//UE_LOG(LogTemp, Error, TEXT("%s : PushCommand_Default :: StartCommand"), *GetOwner()->GetName());
			OnStartCurrentCommand();
			return true;
		}
		else
		{
			FinishCommand();
			return true;
		}
	}
	ResetCommandQue();
	return false;
}

bool UPalCommandComponent::IsValidCommand()
{
	return FPalCommand::IsValidCommand(CurrentCommandData);
}

void UPalCommandComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	UActorComponent::TickComponent(DELTA, TickType, ThisTickFunction);
}

void UPalCommandComponent::FinishCommand()
{
	FPalCommand command = CurrentCommandData;
	if (CurrentExcute)
	{
		CurrentExcute->Abort();
	}
	if (CurrentExcute)
	{
	CurrentExcute->ResetStarted();
	}
	CurrentCommandData.Reset();
	OnFinishedCurrentCommand();
	if (OnCommandFinished.IsBound())
	{
		OnCommandFinished.Broadcast(GetOwner(), command);
	}
}

void UPalCommandComponent::ResetCommandQue()
{
	UPalCommandComponent::FinishCommand();
}
