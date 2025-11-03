#include "Creature/Component/CreatureAction_Attack.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/Character.h"
#include "Engine/DamageEvents.h"
#include "Creature/Interface/CreatureAttackInterface.h"

UCreatureAction_Attack::UCreatureAction_Attack()
{
	Action = ECreatureActionType::Action_Attack;
}

void UCreatureAction_Attack::ActionStart_Implementation(ECreatureActionType ActionType, UObject* TargetObject)
{	
	if (TargetObject != TargetPawn)
	{
		bActionStart = false;
		CurAttackIndex = ECreatureAttackIndex::CreatureAttackIndex_None;
		CurAttackData = MapAttackData.Find(CurAttackIndex);
	}
	TargetPawn = Cast< APawn>(TargetObject);
	if (!TargetPawn || !OwnerController || bActionStart)
		return;
	bActionStart = true;
	MoveToTarget();
}

void UCreatureAction_Attack::ActionEnd_Implementation()
{
	OwnerController->StopMovement();
	CurAttackIndex = ECreatureAttackIndex::CreatureAttackIndex_None;
	CurAttackData = MapAttackData.Find(CurAttackIndex);
	bActionStart = false;
}

bool UCreatureAction_Attack::GetAttackDamage(float& fDamage) const
{
	if (CurAttackData)
	{
		fDamage = CurAttackData->AttackDamage;
		return true;
	}
	return false;
}

bool UCreatureAction_Attack::GetAttackDelay(float& fTime) const
{
	if (CurAttackData)
	{
		fTime = CurAttackData->AttackDelay;
		return true;
	}
	return false;
}

void UCreatureAction_Attack::BeginPlay()
{
	UCreatureActionComponent::BeginPlay();
	if (OwnerController)
		OwnerController->ReceiveMoveCompleted.AddDynamic(this, &UCreatureAction_Attack::FinishMoved);
	MapAttackData.GetKeys(ArrayAttackIndex);
}

void UCreatureAction_Attack::SetRandomIndex()
{
	int i = (rand() % (ArrayAttackIndex.Num() - 1 )) + 1;
	CurAttackIndex = (ECreatureAttackIndex)i;
	CurAttackData = MapAttackData.Find(CurAttackIndex);
}

void UCreatureAction_Attack::MoveToTarget()
{
	if (!TargetPawn)
	{
		bActionStart = false;
	}
	if (!bActionStart)
		return;

	FAIMoveRequest MoveReq(TargetPawn.Get());
	MoveReq.SetUsePathfinding(true);
	MoveReq.SetAllowPartialPath(true);
	MoveReq.SetAcceptanceRadius(100.0f);
	MoveReq.SetReachTestIncludesAgentRadius(true);
	MoveReq.SetCanStrafe(true);
	FNavPathSharedPtr Path{};
	OwnerController->MoveTo(MoveReq, &Path);
	if (!Path.IsValid())
	{
		OwnerController->StopMovement();
		bActionStart = false;
	}
}

void UCreatureAction_Attack::FinishMoved(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (!bActionStart && CurAttackIndex != ECreatureAttackIndex::CreatureAttackIndex_None)
		return;

	if (EPathFollowingResult::Type::Success == Result && TargetPawn && bActionStart) // && AttackComponent)
	{
		SetRandomIndex();
		if (StartDelegate.IsBound())
		{
			StartDelegate.Broadcast(CurAttackIndex);
		}
		//FTimerHandle Handle{ };
		//GetWorld()->GetTimerManager().SetTimer(Handle, this, &UCreatureAction_Attack::MoveToTarget, 3.0f);
	}
	else if (!TargetPawn || EPathFollowingResult::Type::Success != Result)
	{
		MoveToTarget();
	}
}

