#include "Creature/Component/CreatureAction_Resource.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/Character.h"


UCreatureAction_Resource::UCreatureAction_Resource()
{
	Action = ECreatureActionType::Action_Resource;
}

bool UCreatureAction_Resource::ActionStart_Implementation(ECreatureActionType ActionType, UObject* TargetObject)
{
	return false;
}

bool UCreatureAction_Resource::ActionEnd_Implementation()
{
	return false;
}

void UCreatureAction_Resource::BeginPlay()
{
	UCreatureActionComponent::BeginPlay();
}
