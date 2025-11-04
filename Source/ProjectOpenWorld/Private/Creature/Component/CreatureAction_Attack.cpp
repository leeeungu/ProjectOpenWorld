#include "Creature/Component/CreatureAction_Attack.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/Character.h"
#include "Engine/DamageEvents.h"
#include "Creature/Interface/CreatureAttackInterface.h"

UCreatureAction_Attack::UCreatureAction_Attack()
{
	Action = ECreatureActionType::Action_Attack;
	IsAttackable = true;
	MapAttackData.Empty();
	MapAttackData.Add({ ECreatureAttackIndex::CreatureAttackIndex_Default	,FCreatureAttackData {} });
}

void UCreatureAction_Attack::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	UCreatureActionComponent::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bActionStart)
	{
		AttackTime -= DeltaTime;
		if (AttackTime < 0 || FMath::IsNearlyZero(AttackTime))
		{
			SetRandomIndex();
			if (StartDelegate.IsBound())
			{
				StartDelegate.Broadcast(CurAttackIndex);
			}
		}
	}
}

bool UCreatureAction_Attack::ActionStart_Implementation(ECreatureActionType ActionType, UObject* TargetObject)
{	
	if (Cast< APawn>(TargetObject) != TargetPawn)
	{
		bActionStart = false;
		CurAttackIndex = ECreatureAttackIndex::CreatureAttackIndex_None;
		CurAttackData = nullptr;
		IsAttackable = true;
		AttackTime = 0.0f;
	}
	TargetPawn = Cast< APawn>(TargetObject);
	if (!TargetPawn || !IsAttackable)
	{
		return false;
	}
	bActionStart = true;
	SetRandomIndex();
	if (StartDelegate.IsBound())
	{
		StartDelegate.Broadcast(CurAttackIndex);
	}
	return true;
}

bool UCreatureAction_Attack::ActionEnd_Implementation()
{
	if (bActionStart == false)
		return false;
	bActionStart = false; 
	TargetPawn = nullptr;
	IsAttackable = true;
	CurAttackIndex = ECreatureAttackIndex::CreatureAttackIndex_None;
	CurAttackData = nullptr;
	AttackTime = 0.0f;
	return true;
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
	MapAttackData.GetKeys(ArrayAttackIndex);
}

void UCreatureAction_Attack::SetRandomIndex()
{
	if (ArrayAttackIndex.Num() == 0)
	{
		bActionStart = false;
		CurAttackIndex = ECreatureAttackIndex::CreatureAttackIndex_None;
		CurAttackData = nullptr;
		IsAttackable = true;
		return;
	}
	IsAttackable = false;
	int i = (rand() % ArrayAttackIndex.Num());
	CurAttackIndex = (ECreatureAttackIndex)ArrayAttackIndex[i];
	CurAttackData = MapAttackData.Find(CurAttackIndex);
	AttackTime = 0.0f;
	if (CurAttackData)
	{
		AttackTime = CurAttackData->AttackDelay;
	}
}
