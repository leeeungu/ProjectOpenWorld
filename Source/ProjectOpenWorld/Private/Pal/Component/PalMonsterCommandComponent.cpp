#include "Pal/Component/PalMonsterCommandComponent.h"
#include "Pal/CommandExecutor/PalCommandExecutor_MoveLocation.h"
#include "Pal/CommandExecutor/PalCommandExecutor_MoveActor.h"
#include "Pal/CommandExecutor/PalCommandExecutor_MonsterAttack.h"

void UPalMonsterCommandComponent::BeginPlay()
{
	UPalCommandComponent::BeginPlay();
	CommandExecutors.Init(TArray<TStrongObjectPtr< UPalCommandExecutorBase>>{}, (uint8)EPalCommandKind::Max_PalCommandKind);
	CommandExecutors[(uint8)EPalCommandKind::Move].Init(nullptr, (uint8)ESubMoveType::Max_MoveType);
	CommandExecutors[(uint8)EPalCommandKind::Attack].Init(nullptr, (uint8)ESubAttackType::Max_AttackType);
	CommandExecutors[(uint8)EPalCommandKind::Work].Init(nullptr, 0);

	CreateNewExcutor<UPalCommandExecutor_MoveLocation>(EPalCommandKind::Move, (uint8)ESubMoveType::Location, TEXT("MoveLocation"));
	CreateNewExcutor<UPalCommandExecutor_MoveActor>(EPalCommandKind::Move, (uint8)ESubMoveType::Actor, TEXT("MoveActor"));
	CreateNewExcutor<UPalCommandExecutor_MonsterAttack>(EPalCommandKind::Attack, 0, TEXT("Attack"));
	CommandExecutors[(uint8)EPalCommandKind::Attack][1] = CommandExecutors[(uint8)EPalCommandKind::Attack][2] = CommandExecutors[(uint8)EPalCommandKind::Attack][0];
}
