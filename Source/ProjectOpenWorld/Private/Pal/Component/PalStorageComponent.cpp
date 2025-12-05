#include "Pal/Component/PalStorageComponent.h"

UPalStorageComponent::UPalStorageComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPalStorageComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPalStorageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPalStorageComponent::StorePal(AActor* NewPal)
{
	PalStorage.Add(NewPal, FPalStorageData{});
}

