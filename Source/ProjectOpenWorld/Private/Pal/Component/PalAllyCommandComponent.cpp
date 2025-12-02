#include "Pal/Component/PalAllyCommandComponent.h"
#include "Pal/CommandExecutor/PalCommandExecutorBase.h"
#include "Pal/CommandExecutor/PalCommandExecutor_Architecture.h"
#include "Pal/CommandExecutor/PalCommandExecutor_MoveLocation.h"
#include "Pal/CommandExecutor/PalCommandExecutor_MoveActor.h"

void UPalAllyCommandComponent::BeginPlay()
{
	UPalCommandComponent::BeginPlay();
	CommandExecutors.Init(TArray<TObjectPtr< UPalCommandExecutorBase>>{}, (uint8)EPalCommandKind::Max_PalCommandKind);
	CommandExecutors[(uint8)EPalCommandKind::Move].Init(nullptr, (uint8)ESubMoveType::Max_MoveType);
	CommandExecutors[(uint8)EPalCommandKind::Attack].Init(nullptr, (uint8)ESubAttackType::Max_AttackType);
	CommandExecutors[(uint8)EPalCommandKind::Work].Init(nullptr, (uint8)ESubWorkType::Max_WorkType);

	UPalCommandExecutorBase* Base{};
	Base = CommandExecutors[(uint8)EPalCommandKind::Move][(uint8)ESubMoveType::Location] = NewObject<UPalCommandExecutor_MoveLocation>(this,TEXT("MoveLocation"));
	Base->Initialize(this);
	Base = CommandExecutors[(uint8)EPalCommandKind::Move][(uint8)ESubMoveType::Actor] =  NewObject<UPalCommandExecutor_MoveActor>(this, TEXT("MoveActor"));
	Base->Initialize(this);
	Base = CommandExecutors[(uint8)EPalCommandKind::Work][(uint8)ESubWorkType::Architecture] = NewObject<UPalCommandExecutor_Architecture>(this, TEXT("Architecture"));
	Base->Initialize(this);
	Base = nullptr;
}

void UPalAllyCommandComponent::OnStartCurrentCommand()
{
	if (IsValidCommand() == false)
		return;
	FPalCommand* Current = GetCurrentCommand_C();
	uint8 idx = (uint8)Current->CommandKind;
	if (!CommandExecutors.IsValidIndex(idx) || !CommandExecutors[idx].IsValidIndex(Current->SubCommandType))
		return;

	CurrentExcute = CommandExecutors[(uint8)Current->CommandKind][Current->SubCommandType];
	if (CurrentExcute)
	{
		CurrentExcute->StartCommand(*Current);
	}
}

void UPalAllyCommandComponent::OnFinishedCurrentCommand()
{
	CurrentExcute = nullptr;
}
