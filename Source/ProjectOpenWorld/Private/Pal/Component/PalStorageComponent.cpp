#include "Pal/Component/PalStorageComponent.h"

UPalStorageComponent::UPalStorageComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPalStorageComponent::BeginPlay()
{
	UActorComponent::BeginPlay();
	PalStorage.Init(FPalStoreInventoryData{}, InventorySize);
}

void UPalStorageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	UActorComponent::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UPalStorageComponent::CanStorePal(int Index) const
{
	return PalStorage.IsValidIndex(Index);
}

void UPalStorageComponent::StorePal(FPalStoreInventoryData NewPal, int Index)
{
	if (!CanStorePal(Index))
		return;

	if (!PalStorage[Index].SpawnClass)
	{
		PalStorage[Index] = NewPal;
	}
}

void UPalStorageComponent::RemovePal(FPalStoreInventoryData TargetPal, int index)
{
	PalStorage[index] = FPalStoreInventoryData{};
}

AActor* UPalStorageComponent::SpawnPal(int Index)
{
	AActor* pSpawnedPal = nullptr;
	if (SpawnedPal.Num() + 1 > SpawnSize)
		return pSpawnedPal;
	FPalStoreInventoryData* Data = &PalStorage[Index];
	if (!Data || !Data->SpawnClass)
		return pSpawnedPal;
	FVector location = GetOwner()->GetActorLocation() + FVector(-200, 0, 200);
	pSpawnedPal = GetWorld()->SpawnActor(Data->SpawnClass, &location, nullptr, FActorSpawnParameters{});
	SpawnedPal.Add(pSpawnedPal);
	Data->bSpawned = true;
	return pSpawnedPal;
}
