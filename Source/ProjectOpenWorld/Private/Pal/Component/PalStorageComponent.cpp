#include "Pal/Component/PalStorageComponent.h"
#include "Creature/Character/BaseCreature.h"
#include "GameFramework/CharacterMovementComponent.h"

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
	SpawnedPal.Add(pSpawnedPal, Index);
	Data->bSpawned = true;
	return pSpawnedPal;
}

void UPalStorageComponent::DeSpawnPal(AActor* TargetPal)
{
	if (!TargetPal || !SpawnedPal.Contains(TargetPal))
		return;
	int Index = SpawnedPal[TargetPal];
	FPalStoreInventoryData* Data = &PalStorage[Index];
	if (Data)
	{
		Data->bSpawned = false;
	}
	SpawnedPal.Remove(TargetPal);
}

void UPalStorageComponent::ShowAllSpawnedPals()
{
	for (auto& Pal : SpawnedPal)
	{
		if (Pal.Key)
		{
			Pal.Key->SetActorHiddenInGame(false);
			Pal.Key->UpdateComponentVisibility();
			ACharacter* pCharacter = Cast<ACharacter>(Pal.Key);
			if (pCharacter)
			{
				pCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			}
		}
	}
}

void UPalStorageComponent::HideAllSpawnedPals()
{
	for (auto& Pal : SpawnedPal)
	{
		if (Pal.Key)
		{
			Pal.Key->SetActorHiddenInGame(true);
			ABaseCreature * pCharacter = Cast<ABaseCreature>(Pal.Key);
			if (pCharacter)
			{
				pCharacter->SetActionStarted(false);
				pCharacter->StopAnimMontage();
				pCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
			}
		}
	}
}
