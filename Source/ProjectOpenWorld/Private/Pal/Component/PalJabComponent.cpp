#include "Pal/Component/PalJabComponent.h"

UPalJabComponent::UPalJabComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPalJabComponent::BeginPlay()
{
	Super::BeginPlay();
}