#include "Pal/Component/PalCommandComponent.h"
#include "Pal/Controller/PalAIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/Character.h"
//
//UPalCommandComponent_Pre::UPalCommandComponent_Pre()
//{
//	PrimaryComponentTick.bCanEverTick = true;
//	SetPushCommandFunc(&UPalCommandComponent_Pre::PushCommand_Default);
//	ResetCurrentCommand();
//	SetComponentTickEnabled(false);
//}
//
//void UPalCommandComponent_Pre::ResetCommand(FPalCommand& pData)
//{
//	pData.Reset();
//}
//
//void UPalCommandComponent_Pre::ResetCurrentCommand()
//{
//	ResetCommand(DummyCommand);
//	if (CurrentCommand)
//		ResetCommand(*CurrentCommand);
//	CurrentCommand = &DummyCommand;
//	CurrentCommandData = *CurrentCommand;
//	CurrentExcute = nullptr;
//}
//
//void UPalCommandComponent_Pre::BeginPlay()
//{
//	UActorComponent::BeginPlay();
//	QueueEmpty.Empty();
//	QueueCommand.Empty();
//	CommandPool.Init(FPalCommand{}, CommandSize::MaxSize);
//	for (int i = 0; i < CommandSize::MaxSize; i++)
//	{
//		QueueEmpty.Enqueue(&CommandPool[i]);
//	}
//	ACharacter* OwnerPal = Cast<ACharacter>(GetOwner());
//	if (OwnerPal)
//	{
//		APalAIController* OwnerController = Cast<APalAIController>(OwnerPal->GetController());
//		if (OwnerController)
//		{
//			OwnerController->ReceiveMoveCompleted.AddUniqueDynamic(this, &UPalCommandComponent_Pre::FinishMove);
//		}
//	}
//}
//
//bool UPalCommandComponent_Pre::PopCommand()
//{
//	if (CurrentCommand != &DummyCommand)
//	{
//		UE_LOG(LogTemp, Error, TEXT("%s : Current Command Not Finished"), *GetOwner()->GetName());
//		return false;
//	}
//
//	if (QueueCommand.Dequeue(CurrentCommand) == false)
//	{
//		SetComponentTickEnabled(false);
//		ResetCommand(*CurrentCommand);
//		ResetCurrentCommand();	
//		LastCommand = nullptr;
//		UE_LOG(LogTemp, Log, TEXT("%s : Command None"), *GetOwner()->GetName());
//		return false;
//	}
//	if (IsValidCommand() == false)
//	{
//		UE_LOG(LogTemp, Error, TEXT("%s : Command Invalid"), *GetOwner()->GetName());
//		return false;
//	}
//
//	const FPalCommand* Current = GetCurrentCommand_C();
//	uint8 idx = (uint8)Current->CommandKind;
//	if (!CommandExecutors.IsValidIndex(idx) || !CommandExecutors[idx].IsValidIndex(Current->SubCommandType))
//	{
//		UE_LOG(LogTemp, Error, TEXT("%s : Command Executor Not Found"), *GetOwner()->GetName());
//		return false;
//	}
//	if (CurrentExcute == CommandExecutors[(uint8)Current->CommandKind][Current->SubCommandType].Get())
//	{
//		UE_LOG(LogTemp, Error, TEXT("%s : Same Command Executor Running"), *GetOwner()->GetName());
//		return false;
//	}
//
//	CurrentExcute = CommandExecutors[(uint8)Current->CommandKind][Current->SubCommandType].Get();
//	if (CurrentExcute)
//	{
//		if (CurrentExcute->StartCommand(*Current))
//		{
//			CurrentCommandData = *CurrentCommand;
//			OnStartCurrentCommand();
//			return true;	
//		}
//		else
//		{
//			FinishCommand();
//			return true;
//		}
//	}
//	UE_LOG(LogTemp, Error, TEXT("%s : Command Executor is null"), *GetOwner()->GetName());
//	return false;
//}
//
//bool UPalCommandComponent_Pre::PushCommand_Default(const FPalCommand& NewCommand)
//{
//	FPalCommand* pEmpthy{};
//	if (LastCommand)
//	{
//		if (NewCommand == *LastCommand)
//		{
//			UE_LOG(LogTemp, Log, TEXT("%s : Duple Command"), *GetOwner()->GetName());
//			return true;
//		}
//	}
//	if (QueueEmpty.Dequeue(pEmpthy) == false)
//	{
//		UE_LOG(LogTemp, Log, TEXT("%s : Command Full"), *GetOwner()->GetName());
//		PopCommand();
//		return false;
//	}
//	*pEmpthy = NewCommand;
//	LastCommand = pEmpthy;
//	QueueCommand.Enqueue(pEmpthy);
//	SetComponentTickEnabled(true);
//	return PopCommand();
//}
//
//bool UPalCommandComponent_Pre::PushCommand_DequqOld(const FPalCommand& NewCommand) // 미사용
//{
//	FPalCommand* pEmpthy{};
//	if (QueueEmpty.Dequeue(pEmpthy) == false)
//	{
//		if (QueueCommand.Dequeue(pEmpthy) == false)
//			return false;
//	
//		return 	PopCommand();
//	}
//	pEmpthy->CommandKind = NewCommand.CommandKind;
//	pEmpthy->pInstigatorActor = NewCommand.pInstigatorActor;
//	pEmpthy->pTarget = NewCommand.pTarget;
//	QueueCommand.Enqueue(pEmpthy);
//	SetComponentTickEnabled(true);
//	return PopCommand();
//}
//
//void UPalCommandComponent_Pre::SetPushCommandFunc(bool(UPalCommandComponent_Pre::* Func)(const FPalCommand&))
//{
//	PushCommandFunc = Func;
//}
//
//void UPalCommandComponent_Pre::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//	if (!QueueCommand.IsEmpty() && !IsValidCommand())
//	{
//		PopCommand();
//	}
//}
//

//
//void UPalCommandComponent_Pre::FinishCommand()
//{
//	if (CurrentCommand != &DummyCommand)
//	{
//		if (CurrentExcute)
//			CurrentExcute->Abort();
//		if (CurrentExcute)
//			CurrentExcute->ResetStarted();
//		if (LastCommand == CurrentCommand)
//		{
//			LastCommand = nullptr;
//		}
//		FPalCommand command = *CurrentCommand;
//		QueueEmpty.Enqueue(CurrentCommand);
//		ResetCurrentCommand();
//		OnFinishedCurrentCommand();
//		if (CurrentCommand != &DummyCommand)
//			UE_LOG(LogTemp, Error, TEXT("%s : Current Command Not Reset After Finish"), *GetOwner()->GetName());
//		if (OnCommandFinished.IsBound())
//		{
//			OnCommandFinished.Broadcast(GetOwner(), command);
//		}
//	}
//	PopCommand();
//}
//
//void UPalCommandComponent_Pre::ResetCommandQue()
//{
//	FinishCommand();
//	QueueCommand.Empty();
//	QueueEmpty.Empty();
//	for (int i = 0; i < CommandSize::MaxSize; i++)
//	{
//		ResetCommand(CommandPool[i]);
//		QueueEmpty.Enqueue(&CommandPool[i]);
//	}
//	LastCommand = nullptr;
//	ResetCommand(DummyCommand);
//	if (CurrentExcute)
//	{
//		CurrentExcute->Abort();
//		CurrentExcute->ResetStarted();
//	}
//	ResetCurrentCommand();
//	UE_LOG(LogTemp, Log, TEXT("%s : Command Queue Reset"), *GetOwner()->GetName());
//}
//

#include "Pal/Component/Work/PalWorkComponent_Attack.h"
#include "Pal/Component/Work/PalWorkComponent_Architecture.h"
#include "Pal/Component/Work/PalWorkComponent_Location.h"
#include "Pal/Component/Work/PalWorkComponent_Mining.h"

bool UPalCommandComponent::PushCommand_Default(const FPalCommand& NewCommand)
{
	CurrentCommand = &CurrentCommandData;
	if (CurrentCommandData == NewCommand || FPalCommand::IsValidCommand(CurrentCommandData))
		return false;
	const FPalCommand* Current = &NewCommand;
	uint8 idx = (uint8)Current->CommandKind;

/*	if (CommandExecutors.Find(Current->CommandKind) && Current->CommandKind == EPalCommandKind::Attack
		&& CommandExecutors[Current->CommandKind].IsValidIndex(0))
	{
		CurrentExcute = CommandExecutors[Current->CommandKind][0];
	}
	else */
	if (!CommandExecutors.Find(Current->CommandKind) || !CommandExecutors[Current->CommandKind].IsValidIndex(Current->SubCommandType))
	{
		UE_LOG(LogTemp, Error, TEXT("%s : Command Executor Not Found"), *GetOwner()->GetName());
		return false;
	}
	//if(!CurrentExcute)
	CurrentExcute = CommandExecutors[Current->CommandKind][Current->SubCommandType];
	if (CurrentExcute && CurrentExcute->CanWork() )
	{
		CurrentCommandData = NewCommand;
		CurrentExcute->WorkStart(CurrentCommandData);
		return true;
	}
	CurrentExcute = nullptr;
	return false;
}

void UPalCommandComponent::BeginPlay()
{
	UActorComponent::BeginPlay();
	ACharacter* OwnerPal = Cast<ACharacter>(GetOwner());
	if (OwnerPal)
	{
		APalAIController* OwnerController = Cast<APalAIController>(OwnerPal->GetController());
		if (OwnerController)
		{
			OwnerController->ReceiveMoveCompleted.AddUniqueDynamic(this, &UPalCommandComponent::FinishMove);
		}
	}
	CommandExecutors.FindOrAdd(EPalCommandKind::Work).AddZeroed((uint8)ESubWorkType::Max_WorkType);
	CommandExecutors.FindOrAdd(EPalCommandKind::Attack).AddZeroed((uint8)ESubAttackType::Max_AttackType);
	CommandExecutors.FindOrAdd(EPalCommandKind::Move).AddZeroed((uint8)ESubMoveType::Max_MoveType);

	UPalWorkComponent * WorkComponent = NewObject<UPalWorkComponent_Architecture>(GetOwner());
	WorkComponent->RegisterComponent();
	CommandExecutors[EPalCommandKind::Work][(uint8)ESubWorkType::Architecture] = WorkComponent;
	WorkComponent = NewObject<UPalWorkComponent_Mining>(GetOwner());
	WorkComponent->RegisterComponent();
	CommandExecutors[EPalCommandKind::Work][(uint8)ESubWorkType::Mining] = WorkComponent;
	WorkComponent = NewObject<UPalWorkComponent_Location>(GetOwner());
	WorkComponent->RegisterComponent();
	CommandExecutors[EPalCommandKind::Move][(uint8)ESubMoveType::Location] = WorkComponent;
	WorkComponent = NewObject<UPalWorkComponent_Attack>(GetOwner());
	WorkComponent->RegisterComponent();
	for (uint8 i = 0; i < (uint8)ESubAttackType::Max_AttackType; i++)
	{
		CommandExecutors[EPalCommandKind::Attack][i] = WorkComponent;
	}
}

UPalCommandComponent::UPalCommandComponent() :Super()
{
	CurrentCommand = &CurrentCommandData;
}

bool UPalCommandComponent::IsValidCommand()
{
	return FPalCommand::IsValidCommand(CurrentCommandData);
}

void UPalCommandComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	UActorComponent::TickComponent(DELTA, TickType, ThisTickFunction);
}

bool UPalCommandComponent::PushCommand(const FPalCommand& NewCommand)
{
	return PushCommand_Default(NewCommand);
}

void UPalCommandComponent::FinishCommand()
{
	if (CurrentExcute)
	{
		CurrentExcute->WorkEnd(CurrentCommandData);
		CurrentExcute = nullptr;
		//CurrentExcute->Abort();
	}
	//if (CurrentExcute)
	//{
	////CurrentExcute->ResetStarted();
	//}
	FPalCommand command = CurrentCommandData;
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

void UPalCommandComponent::FinishMove(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (!CurrentExcute)// || Result == EPathFollowingResult::Type::Invalid || Result == EPathFollowingResult::Type::OffPath)
	{
		return;
	}
	if ( Result == EPathFollowingResult::Type::Success)
	{
		CurrentExcute->WorkEvent(CurrentCommandData);
	}
	else if (Result == EPathFollowingResult::Invalid)
	{
		FinishCommand();
		ACharacter* OwnerPal = Cast<ACharacter>(GetOwner());
		if (OwnerPal)
		{
			APalAIController* OwnerController = Cast<APalAIController>(OwnerPal->GetController());
			if (OwnerController)
			{
				OwnerController->ResetMove();
			}
		}
	}
	else
	{
		CurrentExcute->WorkStart(CurrentCommandData);
	}
}