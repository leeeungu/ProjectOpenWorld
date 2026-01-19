#include "Landscape/Component/GemerateStaticObjectComponent.h"

UGemerateStaticObjectComponent::UGemerateStaticObjectComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGemerateStaticObjectComponent::BeginPlay()
{
	Super::BeginPlay();
}

//
//// Called every frame
//void UGemerateStaticObjectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}
//
