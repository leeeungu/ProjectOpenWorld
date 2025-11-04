#include "Creature/Component/CreatureAction_Resource.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/Character.h"
#include "Resource/Actor/ResourceActor.h"
#include "Creature/Character/BaseCreature.h"


UCreatureAction_Resource::UCreatureAction_Resource()
{
	Action = ECreatureActionType::Action_Resource;
}

bool UCreatureAction_Resource::ActionStart_Implementation(ECreatureActionType ActionType, UObject* TargetObject)
{
	if (TargetActor != TargetObject)
	{
		TargetActor = nullptr;
		bActionStart = false;
	}
	TargetActor = Cast< AResourceActor>(TargetObject);
	if (bActionStart || !TargetActor)
	{
		return false;
	}
	TargetActor->onExtractEnd.AddDynamic(this, &UCreatureAction_Resource::ResetAction);
	bActionStart = true;
	return true;
}

bool UCreatureAction_Resource::ActionEnd_Implementation()
{
	if (!bActionStart)
		return false;
	if (TargetActor)
		TargetActor->onExtractEnd.RemoveDynamic(this, &UCreatureAction_Resource::ResetAction);
	TargetActor = nullptr;
	bActionStart = false;
	return true;
}

void UCreatureAction_Resource::BeginPlay()
{
	UCreatureActionComponent::BeginPlay();
}

void UCreatureAction_Resource::ExtractResource()
{
	if (!TargetActor || !bActionStart)
		return;
	TargetActor->SpawnRandomItem();
}


void UCreatureAction_Resource::ResetAction()
{
	if (ABaseCreature* pOwner = Cast< ABaseCreature>(GetOwner()))
	{
		pOwner->ResetAction();
	}
	TargetActor = nullptr;
	bActionStart = false;
}