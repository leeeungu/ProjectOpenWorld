#include "Pal/Component/PalSpawnerComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Pal/Character/PalBaseCreature.h"

UPalSpawnerComponent::UPalSpawnerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPalSpawnerComponent::SwapSpawnedPals(int32 Src, int32 Dst)
{
	Swap< FContainerData>(PalSpawned[Src], PalSpawned[Dst]);
	//AActor* pSrc = PalSpawned[Src].Pal;
	//AActor* pDst = PalSpawned[Dst].Pal;
	//PalSpawned[Src] = PalSpawned[Dst];
	//PalSpawned[Dst] = pSrc;
	OnContainerUpdated.Broadcast(Src, PalSpawned[Dst].Pal);
	OnContainerUpdated.Broadcast(Dst, PalSpawned[Src].Pal);
}

void UPalSpawnerComponent::BeginPlay()
{
	Super::BeginPlay();
	{
		PalSpawned.Init({}, SpawnerSize);
	}
}

void UPalSpawnerComponent::SpawnPal(int32 Index)
{
	if (!PalSpawned.IsValidIndex(Index) || PalSpawned[Index].bSpawned)
		return;
	AActor* PalToSpawn = PalSpawned[Index].Pal;
	if (PalToSpawn)
	{
		PalSpawned[Index].bSpawned = true;
		PalToSpawn->SetActorLocation(GetComponentLocation());
		APalBaseCreature* pCreature = Cast<APalBaseCreature>(PalToSpawn);
		if (pCreature)
		{
			pCreature->VisibleCharacter();
		}
	}
}

bool UPalSpawnerComponent::StorePal(AActor* TargetActor, int32 Index)
{
	if (PalSpawned.IsValidIndex(Index) && !PalSpawned[Index].Pal)
	{
		AActor* Pre = PalSpawned[Index].Pal;
		PalSpawned[Index].Pal = TargetActor;
		OnContainerUpdated.Broadcast(Index, Pre);
		return true;
	}
	return false;
}

void UPalSpawnerComponent::RemovePal(int32 Index)
{
	if (PalSpawned.IsValidIndex(Index))
	{
		AActor* Pre = PalSpawned[Index].Pal;
		PalSpawned[Index].Pal = nullptr;
		if(PalSpawned[Index].bSpawned)
		{
			PalSpawned[Index].bSpawned = false;
			APalBaseCreature* pCreature = Cast<APalBaseCreature>(Pre);
			if (pCreature)
			{
				if (pCreature)
				{
					pCreature->StopAnimMontage();
					pCreature->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
				}
				pCreature->HideCharacter();
			}
		}
		OnContainerUpdated.Broadcast(Index, Pre);
	}
}


AActor* UPalSpawnerComponent::GetPal(int32 Index) const
{
	if (PalSpawned.IsValidIndex(Index))
	{
		return PalSpawned[Index].Pal;
	}
	return nullptr;
}

//
//void UPalSpawnerComponent::DeSpawnPal(int InventoryIndex, int SpawnIndex)
//{
//	if (!PalSpawned.IsValidIndex(SpawnIndex))
//		return;
//	AActor* PalToDeSpawn = PalSpawned[SpawnIndex];
//	if (PalToDeSpawn)
//	{
//		PalToDeSpawn->SetActorHiddenInGame(true);
//		PalToDeSpawn->SetActorTickEnabled(false);
//		APalBaseCreature* pCreature = Cast<APalBaseCreature>(PalToDeSpawn);
//		if (pCreature)
//		{
//			//pCreature->SetActionStarted(false);
//			pCreature->StopAnimMontage();
//			pCreature->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
//		}
//	}
//	PalSpawned[SpawnIndex] = nullptr;
//	if (PalStorage.IsValidIndex(InventoryIndex))
//	{
//		PalStorage[InventoryIndex] = PalToDeSpawn;
//		OnPalStoreChanged.Broadcast(InventoryIndex, PalStorage[InventoryIndex]);
//	}
//	OnPalSpawnChanged.Broadcast(SpawnIndex, PalSpawned[SpawnIndex]);
//
//
//	APalBaseCamp* BaseCamp = Cast<APalBaseCamp>(GetOwner());
//	if (BaseCamp)
//	{
//		BaseCamp->DeSpawnPal(PalToDeSpawn);
//	}
//
//}
//
//void UPalSpawnerComponent::SwapSpawnedPals(int IndexA, int IndexB)
//{
//	if (PalSpawned.IsValidIndex(IndexA) && PalSpawned.IsValidIndex(IndexB))
//	{
//		TObjectPtr<AActor> Temp = PalSpawned[IndexA];
//		PalSpawned[IndexA] = PalSpawned[IndexB];
//		PalSpawned[IndexB] = Temp;
//		OnPalSpawnChanged.Broadcast(IndexA, PalSpawned[IndexA]);
//		OnPalSpawnChanged.Broadcast(IndexB, PalSpawned[IndexB]);
//	}
//}
//
//void UPalSpawnerComponent::ShowAllSpawnedPals()
//{
//	for (auto& Pal : PalSpawned)
//	{
//		if (Pal)
//		{
//			Pal->SetActorHiddenInGame(false);
//			Pal->SetActorTickEnabled(true);
//			ACharacter* pCreature = Cast<ACharacter>(Pal);
//			if (pCreature)
//			{
//				pCreature->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
//			}
//		}
//	}
//}
//
//void UPalSpawnerComponent::HideAllSpawnedPals()
//{
//	for (auto& Pal : PalSpawned)
//	{
//		if (Pal)
//		{
//			Pal->SetActorHiddenInGame(true);
//			Pal->SetActorTickEnabled(false);
//			APalBaseCreature* pCreature = Cast<APalBaseCreature>(Pal);
//			if (pCreature)
//			{
//				//pCreature->SetActionStarted(false);
//				pCreature->StopAnimMontage();
//				pCreature->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
//			}
//		}
//	}
//}