#include "Interaction/Component/PalInteractionComponent.h"
#include "Interaction/InteractionInterface.h"


UPalInteractionComponent::UPalInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPalInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}