#include "Pal/Component/PalStorageComponent.h"
#include "Pal/Character/PalBaseCreature.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameBase/GameMode/BaseGameInstance.h"
#include "Engine/World.h"

UPalStorageComponent::UPalStorageComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPalStorageComponent::BeginPlay()
{
	UActorComponent::BeginPlay();
	PalStorage.Init(nullptr, InventorySize);
}

void UPalStorageComponent::PalDead(AActor* DeadPal)
{

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

void UPalStorageComponent::OnPreSave()
{
	SavedStorage.Reset();
	for (int32 i = 0; i < PalStorage.Num(); ++i)
	{
		AActor* Pal = PalStorage[i];
		if (!Pal)
			continue;

		FPalStorageSaveData Rec;
		Rec.SlotIndex = i;
		Rec.PalClass = FSoftClassPath(Pal->GetClass());
		SavedStorage.Add(Rec);
	}
	UE_LOG(LogTemp, Warning, TEXT("UPalStorageComponent::OnPreSave %s"), *GetOwner()->GetName());
}

void UPalStorageComponent::OnLoaded()
{
	UE_LOG(LogTemp, Warning, TEXT("UPalStorageComponent::OnLoaded %s"), *GetOwner()->GetName());
	UWorld* World = GetWorld();
	if (!World)
		return;
	if (PalStorage.Num() < InventorySize) 
		PalStorage.Init(nullptr, InventorySize);

	for (const FPalStorageSaveData& Rec : SavedStorage)
	{
		if (!PalStorage.IsValidIndex(Rec.SlotIndex))
			continue;
		UClass* Cls = Rec.PalClass.TryLoadClass<AActor>();
		if (!Cls)
			continue;

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		AActor* NewPal = World->SpawnActor<AActor>(Cls, FTransform::Identity, Params);
		if (!NewPal)
			continue;
		StorePal(FPalStoreInventoryData{ NewPal, Rec.SlotIndex });
	}
}
