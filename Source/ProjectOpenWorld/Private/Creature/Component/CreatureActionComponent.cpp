#include "Creature/Component/CreatureActionComponent.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"

UCreatureActionComponent::UCreatureActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCreatureActionComponent::BeginPlay()
{
	Super::BeginPlay();	
}