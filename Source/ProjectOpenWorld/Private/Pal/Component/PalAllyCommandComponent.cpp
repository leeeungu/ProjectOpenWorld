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
}
