#include "Pal/Component/PalStorageComponent.h"
#include "Creature/Character/BaseCreature.h"
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
	SpawnedPal.Init(nullptr, SpawnSize);
}

void UPalStorageComponent::PalDead(AActor* DeadPal)
{
	for (size_t i = 0; i < SpawnedPal.Num(); i++)
	{
		if (SpawnedPal[i] == DeadPal)
		{
			DeSpawnPal(-1, i);
			break;
		}
	}
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

	int i = 0;
	while(i < InventorySize && PalStorage[i])
	{
		i++;
	}
	if (i < InventorySize)
	{
		AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(NewPal.SpawnActorClass);
		SpawnedActor->OnDestroyed.AddUniqueDynamic(this, &UPalStorageComponent::PalDead);
		PalStorage[i] = SpawnedActor;
		SpawnedActor->SetActorHiddenInGame(true);
		SpawnedActor->SetActorTickEnabled(false);
		ABaseCreature* pCreature = Cast<ABaseCreature>(SpawnedActor);
		if (pCreature)
		{
			pCreature->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		}
		CurrnetInventorySize++;
		OnPalStoreChanged.Broadcast(i, pCreature);
	}
	return true;
}

void UPalStorageComponent::RemovePal(int InventoryIndex)
{
	if(PalStorage.IsValidIndex(InventoryIndex))
	{
		if (PalStorage[InventoryIndex])
		{
			PalStorage[InventoryIndex]->Destroy();
			PalStorage[InventoryIndex] = nullptr;
			CurrnetInventorySize--;
			OnPalStoreChanged.Broadcast(InventoryIndex, PalStorage[InventoryIndex]);
		}
	}
}

AActor* UPalStorageComponent::SpawnPal(int InventoryIndex, int SpawnIndex)
{
	if (!PalStorage.IsValidIndex(InventoryIndex) || !SpawnedPal.IsValidIndex(SpawnIndex))
		return nullptr;
	AActor* PalToSpawn = PalStorage[InventoryIndex];
	if (PalToSpawn)
	{
		PalToSpawn->SetActorHiddenInGame(false);
		PalToSpawn->SetActorTickEnabled(true);
		ABaseCreature* pCreature = Cast<ABaseCreature>(PalToSpawn);
		if (pCreature)
		{
			pCreature->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}
	}
	PalStorage[InventoryIndex] = nullptr;
	OnPalStoreChanged.Broadcast(InventoryIndex, PalStorage[InventoryIndex]);
	if (SpawnedPal[SpawnIndex])
	{
		DeSpawnPal(InventoryIndex, SpawnIndex);
	}
	SpawnedPal[SpawnIndex] = PalToSpawn;
	OnPalSpawnChanged.Broadcast(SpawnIndex, PalToSpawn);
	
	APalBaseCamp* BaseCamp = Cast<APalBaseCamp>(GetOwner());
	if (BaseCamp)
	{
		BaseCamp->SpawnPal(PalToSpawn);
	}

	return PalToSpawn;
}

void UPalStorageComponent::DeSpawnPal(int InventoryIndex, int SpawnIndex)
{
	if (!SpawnedPal.IsValidIndex(SpawnIndex))
		return;
	AActor* PalToDeSpawn = SpawnedPal[SpawnIndex];
	if (PalToDeSpawn)
	{
		PalToDeSpawn->SetActorHiddenInGame(true);
		PalToDeSpawn->SetActorTickEnabled(false);
		ABaseCreature* pCreature = Cast<ABaseCreature>(PalToDeSpawn);
		if (pCreature)
		{
			pCreature->SetActionStarted(false);
			pCreature->StopAnimMontage();
			pCreature->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		}
	}
	SpawnedPal[SpawnIndex] = nullptr;
	if (PalStorage.IsValidIndex(InventoryIndex))
	{
		PalStorage[InventoryIndex] = PalToDeSpawn;
		OnPalStoreChanged.Broadcast(InventoryIndex, PalStorage[InventoryIndex]);
	}
	OnPalSpawnChanged.Broadcast(SpawnIndex, SpawnedPal[SpawnIndex]);


	APalBaseCamp* BaseCamp = Cast<APalBaseCamp>(GetOwner());
	if (BaseCamp)
	{
		BaseCamp->DeSpawnPal(PalToDeSpawn);
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

void UPalStorageComponent::SwapSpawnedPals(int IndexA, int IndexB)
{
	if (SpawnedPal.IsValidIndex(IndexA) && SpawnedPal.IsValidIndex(IndexB))
	{
		TObjectPtr<AActor> Temp = SpawnedPal[IndexA];
		SpawnedPal[IndexA] = SpawnedPal[IndexB];
		SpawnedPal[IndexB] = Temp;
		OnPalSpawnChanged.Broadcast(IndexA, SpawnedPal[IndexA]);
		OnPalSpawnChanged.Broadcast(IndexB, SpawnedPal[IndexB]);
	}
}

void UPalStorageComponent::ShowAllSpawnedPals()
{
	for (auto& Pal : SpawnedPal)
	{
		if (Pal)
		{
			Pal->SetActorHiddenInGame(false);
			Pal->SetActorTickEnabled(true);
			ACharacter* pCreature = Cast<ACharacter>(Pal);
			if (pCreature)
			{
				pCreature->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			}
		}
	}
}

void UPalStorageComponent::HideAllSpawnedPals()
{
	for (auto& Pal : SpawnedPal)
	{
		if (Pal)
		{
			Pal->SetActorHiddenInGame(true);
			Pal->SetActorTickEnabled(false);
			ABaseCreature * pCreature = Cast<ABaseCreature>(Pal);
			if (pCreature)
			{
				pCreature->SetActionStarted(false);
				pCreature->StopAnimMontage();
				pCreature->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
			}
		}
	}
}
