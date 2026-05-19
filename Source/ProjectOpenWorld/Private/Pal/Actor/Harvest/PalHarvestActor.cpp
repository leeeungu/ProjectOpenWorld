#include "Pal/Actor/Harvest/PalHarvestActor.h"
#include "Pal/Component/Harvest/PalHarvestSMComponent.h"

APalHarvestActor::APalHarvestActor()
{
	PrimaryActorTick.bCanEverTick = false;
	MeshComponent = CreateDefaultSubobject< UPalHarvestSMComponent>(TEXT("HarvestMesh"));
	SetRootComponent(MeshComponent);
}

void APalHarvestActor::ResiterWorker(TScriptInterface<IPalWorkerInterface> WorkerClass)
{
	MeshComponent->ResiterWorker(WorkerClass);
}

void APalHarvestActor::UnregisterWorker(TScriptInterface<IPalWorkerInterface> WorkerClass)
{
	MeshComponent->UnregisterWorker(WorkerClass);
}

bool APalHarvestActor::IsWorkable() const
{
	return MeshComponent->IsWorkable();
}
