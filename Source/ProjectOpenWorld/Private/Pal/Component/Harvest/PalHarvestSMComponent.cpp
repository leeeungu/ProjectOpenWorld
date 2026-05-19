#include "Pal/Component/Harvest/PalHarvestSMComponent.h"
#include "Item/AssetUserData/ItemSpawnListAssetUserData.h"
#include "Pal/Interface/PalWorkerInterface.h"
#include "Item/System/ItemDataSubsystem.h"

void UPalHarvestSMComponent::Respawn()
{
	SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SetVisibility(true);
}

void UPalHarvestSMComponent::OnHarvestEvent(FHarvestEventData EventData)
{
	if (UItemSpawnListAssetUserData* SpawnList = Cast< UItemSpawnListAssetUserData>(GetStaticMesh()->GetAssetUserDataOfClass(UItemSpawnListAssetUserData::StaticClass())))
	{
		FItemSpawnRateData SpawnData = SpawnList->GetRandomItem();
		UItemDataSubsystem::SpawnPalStaticItemVisualActorByName(GetWorld(), SpawnData.ItemName, FTransform(EventData.Hit->Location), SpawnData.SpawnRandomCount);
		SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetVisibility(false);
		FTimerHandle Timer{};
		GetOwner()->GetWorldTimerManager().SetTimer(Timer, this, &UPalHarvestSMComponent::Respawn, 3, false, 3);
		for (auto& Worker : Workers)
		{
			if (Worker)
			{
				Worker->EndWorking(true);
			}
		}
	}
}

void UPalHarvestSMComponent::ResiterWorker(TScriptInterface<IPalWorkerInterface> WorkerClass)
{
	Workers.FindOrAdd(WorkerClass);
	WorkerClass->StartWorking();
}

void UPalHarvestSMComponent::UnregisterWorker(TScriptInterface<IPalWorkerInterface> WorkerClass)
{
	Workers.Remove(WorkerClass);
}

bool UPalHarvestSMComponent::IsWorkable() const
{
	return IsVisible();
}