#include "Pal/Actor/Harvest/PalHarvestActor.h"
#include "Pal/Component/Harvest/PalHarvestSMComponent.h"
#include "Pal/Factory/PalCommandFunctionLibrary.h"

APalHarvestActor::APalHarvestActor() :Super{}
{
	PrimaryActorTick.bCanEverTick = false;
	MeshComponent = CreateDefaultSubobject< UPalHarvestSMComponent>(TEXT("HarvestMesh"));
	SetRootComponent(MeshComponent);
}

void APalHarvestActor::ResiterWorker(TScriptInterface<IPalWorkerInterface> WorkerClass)
{
	if(MeshComponent)
		MeshComponent->ResiterWorker(WorkerClass);
}

void APalHarvestActor::UnregisterWorker(TScriptInterface<IPalWorkerInterface> WorkerClass)
{
	if(MeshComponent)
		MeshComponent->UnregisterWorker(WorkerClass);
}

bool APalHarvestActor::IsWorkable() const
{
	if (!MeshComponent)
		return false;
	return MeshComponent->IsWorkable();
}

FPalWorkCommand APalHarvestActor::GetWorkCommand(AActor* InstigatorActor, AActor* Target) const
{
	return UPalCommandFunctionLibrary::WorkMining(InstigatorActor, Target);
}
