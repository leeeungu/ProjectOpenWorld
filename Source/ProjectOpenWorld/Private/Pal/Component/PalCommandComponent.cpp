#include "Pal/Component/PalCommandComponent.h"
#include "Pal/Controller/PalAIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/Character.h"

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
		if (CurrentExcute->CanWork())
		{
			CurrentExcute->WorkStart(CurrentCommandData);
		}
	}
}