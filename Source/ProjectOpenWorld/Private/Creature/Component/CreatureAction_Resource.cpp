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

bool UCreatureAction_Resource::ActionStart_Implementation( AActor* SendActor, AActor* TargetActor)
{
	if (TargetResource != TargetActor)
	{
		TargetResource = nullptr;
		bActionStart = false;
	}
	TargetResource = Cast< AResourceActor>(TargetActor);
	if (bActionStart || !TargetResource)
	{
		return false;
	}
	TargetResource->onExtractEnd.AddDynamic(this, &UCreatureAction_Resource::ResetAction);
	bActionStart = true;
	return true;
}

bool UCreatureAction_Resource::ActionEnd_Implementation()
{
	if (!bActionStart)
		return false;
	if (TargetResource)
		TargetResource->onExtractEnd.RemoveDynamic(this, &UCreatureAction_Resource::ResetAction);
	TargetResource = nullptr;
	bActionStart = false;
	return true;
}

void UCreatureAction_Resource::BeginPlay()
{
	UCreatureActionComponent::BeginPlay();
}

void UCreatureAction_Resource::ExtractResource()
{
	if (!TargetResource || !bActionStart)
		return;
	TargetResource->SpawnRandomItem();
}


void UCreatureAction_Resource::ResetAction()
{
	if (ABaseCreature* pOwner = Cast< ABaseCreature>(GetOwner()))
	{
		pOwner->ResetAction();
	}
	TargetResource = nullptr;
	bActionStart = false;
}