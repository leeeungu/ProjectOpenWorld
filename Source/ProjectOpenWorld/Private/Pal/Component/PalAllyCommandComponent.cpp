#include "Pal/Component/PalAllyCommandComponent.h"
#include "Pal/CommandExecutor/PalCommandExecutor_Architecture.h"
#include "Pal/CommandExecutor/PalCommandExecutor_MoveLocation.h"
#include "Pal/CommandExecutor/PalCommandExecutor_MoveActor.h"
#include "Pal/CommandExecutor/PalCommandExecutor_Transport.h"
#include "Pal/CommandExecutor/PalCommandExecutor_Mining.h"
#include "Pal/CommandExecutor/PalCommandExecutor_Attack.h"

void UPalAllyCommandComponent::BeginPlay()
{
	UPalCommandComponent::BeginPlay();
	CommandExecutors.Init(TArray<TStrongObjectPtr< UPalCommandExecutorBase>>{}, (uint8)EPalCommandKind::Max_PalCommandKind);
	CommandExecutors[(uint8)EPalCommandKind::Move].Init(nullptr, (uint8)ESubMoveType::Max_MoveType);
	CommandExecutors[(uint8)EPalCommandKind::Attack].Init(nullptr, (uint8)ESubAttackType::Max_AttackType);
	CommandExecutors[(uint8)EPalCommandKind::Work].Init(nullptr, (uint8)ESubWorkType::Max_WorkType);

	CreateNewExcutor<UPalCommandExecutor_MoveLocation>(EPalCommandKind::Move, (uint8)ESubMoveType::Location, TEXT("MoveLocation"));
	CreateNewExcutor<UPalCommandExecutor_MoveActor>(EPalCommandKind::Move, (uint8)ESubMoveType::Actor, TEXT("MoveActor"));
	CreateNewExcutor<UPalCommandExecutor_Architecture>(EPalCommandKind::Work, (uint8)ESubWorkType::Architecture, TEXT("Architecture"));
	CreateNewExcutor<UPalCommandExecutor_Transport>(EPalCommandKind::Work, (uint8)ESubWorkType::Transport, TEXT("Transport"));
	CreateNewExcutor<UPalCommandExecutor_Mining>(EPalCommandKind::Work, (uint8)ESubWorkType::Mining, TEXT("Mining"));
	CreateNewExcutor<UPalCommandExecutor_Attack>(EPalCommandKind::Attack, 0, TEXT("Attack"));
	CommandExecutors[(uint8)EPalCommandKind::Attack][1] = CommandExecutors[(uint8)EPalCommandKind::Attack][2] = CommandExecutors[(uint8)EPalCommandKind::Attack][0];

	/*
	UPalCommandExecutorBase* Base = NewObject<>(this, TEXT("MoveLocation"));
	CommandExecutors[(uint8)EPalCommandKind::Move][(uint8)ESubMoveType::Location] = TStrongObjectPtr<UPalCommandExecutorBase>(Base);
	Base->Initialize(this);

	Base = NewObject<UPalCommandExecutor_MoveLocation>(this, TEXT("MoveLocation"));
	Base = CommandExecutors[(uint8)EPalCommandKind::Move][(uint8)ESubMoveType::Actor] =  NewObject<UPalCommandExecutor_MoveActor>(this, TEXT("MoveActor"));
	Base->Initialize(this);

	Base = NewObject<UPalCommandExecutor_MoveLocation>(this, TEXT("MoveLocation"));
	Base = CommandExecutors[(uint8)EPalCommandKind::Work][(uint8)ESubWorkType::Architecture] = NewObject<UPalCommandExecutor_Architecture>(this, TEXT("Architecture"));
	Base->Initialize(this);
	Base = NewObject<UPalCommandExecutor_MoveLocation>(this, TEXT("MoveLocation"));
	Base = CommandExecutors[(uint8)EPalCommandKind::Work][(uint8)ESubWorkType::Transport] = NewObject<UPalCommandExecutor_Transport>(this, TEXT("Transport"));
	Base->Initialize(this);

	Base = CommandExecutors[(uint8)EPalCommandKind::Work][(uint8)ESubWorkType::Mining] = NewObject<UPalCommandExecutor_Mining>(this, TEXT("Mining"));
	Base->Initialize(this);

	Base = CommandExecutors[(uint8)EPalCommandKind::Attack][0] =
	Base->Initialize(this);
	Base = nullptr;*/
}

void UPalAllyCommandComponent::OnStartCurrentCommand()
{
	if (IsValidCommand() == false)
		return;
	const FPalCommand* Current = GetCurrentCommand_C();
	uint8 idx = (uint8)Current->CommandKind;
	if (!CommandExecutors.IsValidIndex(idx) || !CommandExecutors[idx].IsValidIndex(Current->SubCommandType))
		return;

	CurrentExcute = CommandExecutors[(uint8)Current->CommandKind][Current->SubCommandType].Get();
	if (CurrentExcute)
	{
		CurrentExcute->StartCommand(*Current);
	}
}

void UPalAllyCommandComponent::OnFinishedCurrentCommand()
{
	if (CurrentExcute)
	{
		CurrentExcute->Abort();
	}
	CurrentExcute = nullptr;
}
