#include "Pal/Component/PalCommanderComponent.h"

UPalCommanderComponent::UPalCommanderComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPalCommanderComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPalCommanderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

