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
	if (!Cast<APawn>(GetOwner()))
		return;
	OwnerController = Cast<AAIController>(Cast<APawn>(GetOwner())->GetController());
}