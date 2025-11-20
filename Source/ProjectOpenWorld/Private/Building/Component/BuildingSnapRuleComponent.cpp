#include "Building/Component/BuildingSnapRuleComponent.h"

UBuildingSnapRuleComponent::UBuildingSnapRuleComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UBuildingSnapRuleComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBuildingSnapRuleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

