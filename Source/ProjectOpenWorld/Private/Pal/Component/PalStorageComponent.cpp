#include "Pal/Component/PalStorageComponent.h"
#include "Pal/Character/PalBaseCreature.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Pal/Actor/PalBaseCamp.h"

UPalStorageComponent::UPalStorageComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPalStorageComponent::BeginPlay()
{
	UActorComponent::BeginPlay();
	PalStorage.Init(nullptr, InventorySize);
	//PalSpawned.Init(nullptr, SpawnSize);
}

void UPalStorageComponent::PalDead(AActor* DeadPal)
{
	//for (size_t i = 0; i < PalSpawned.Num(); i++)
	//{
	//	if (PalSpawned[i] == DeadPal)
	//	{
	//		DeSpawnPal(-1, i);
	//		break;
	//	}
	//}
}

void UPalStorageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	UActorComponent::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UPalStorageComponent::CanStorePal() const
{
	return CurrnetInventorySize + 1 < InventorySize;
}

bool UPalStorageComponent::StorePal(FPalStoreInventoryData NewPal)
{
	if (!CanStorePal())
		return false;

	int i = NewPal.Index;
	if (!PalStorage.IsValidIndex(i))
	{
		i = 0;
		while (PalStorage.IsValidIndex(i) && PalStorage[i])
		{
			i++;
		}
	}
	if (PalStorage.IsValidIndex(i))
	{
		AActor* PalStore = NewPal.SpawnActor;
		if (PalStore)
		{
			//SpawnedActor->OnDestroyed.AddUniqueDynamic(this, &UPalStorageComponent::PalDead);
			PalStorage[i] = PalStore;
			PalStore->SetActorHiddenInGame(true);
			PalStore->SetActorTickEnabled(false);
			ACharacter* pCreature = Cast<ACharacter>(PalStore);
			if (pCreature)
			{
				pCreature->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
				CurrnetInventorySize++;
			}
			OnPalStoreChanged.Broadcast(i, PalStore);
		}
	}
	return true;
}

void UPalStorageComponent::RemovePal(int InventoryIndex)
{
	if(PalStorage.IsValidIndex(InventoryIndex))
	{
		if (PalStorage[InventoryIndex])
		{
			//PalStorage[InventoryIndex]->Destroy();
			PalStorage[InventoryIndex] = nullptr;
			CurrnetInventorySize--;
			OnPalStoreChanged.Broadcast(InventoryIndex, PalStorage[InventoryIndex]);
		}
	}
}


AActor* UPalStorageComponent::GetStoredPal(int InventoryIndex) const
{
	if (PalStorage.IsValidIndex(InventoryIndex))
	{
		return PalStorage[InventoryIndex];
	}
	return nullptr;
}

void UPalStorageComponent::SwapStoredPals(int IndexA, int IndexB)
{
	if (PalStorage.IsValidIndex(IndexA) && PalStorage.IsValidIndex(IndexB))
	{
		TObjectPtr<AActor> Temp = PalStorage[IndexA];
		PalStorage[IndexA] = PalStorage[IndexB];
		PalStorage[IndexB] = Temp;
		OnPalStoreChanged.Broadcast(IndexA, PalStorage[IndexA]);
		OnPalStoreChanged.Broadcast(IndexB, PalStorage[IndexB]);
	}
}
