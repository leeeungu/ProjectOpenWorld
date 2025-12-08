#include "Pal/Actor/PalBaseCamp.h"
#include "Pal/Component/PalCommanderComponent.h"
#include "Pal/Component/PalStorageComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

APalBaseCamp::APalBaseCamp()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	CampMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	CampMesh->SetupAttachment(RootComponent);
	PalStore = CreateDefaultSubobject<UPalStorageComponent>(TEXT("PalStorage"));
	PalCommander = CreateDefaultSubobject<UPalCommanderComponent>(TEXT("PalCommander"));
	CampBounds = CreateDefaultSubobject <USphereComponent >(TEXT("CampBound"));
	CampBounds->SetupAttachment(RootComponent);
	CampBounds->SetSphereRadius(1200.0f);
}

void APalBaseCamp::BeginPlay()
{
	Super::BeginPlay();
}

void APalBaseCamp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool APalBaseCamp::SpawnPal(TSubclassOf<AActor> TargetPal, int Index)
{
	AActor* Spawned = PalStore->SpawnPal(Index);
	if (Spawned)
	{
		PalCommander->StorePal(Spawned);
		return true;
	}
	return false;
}

void APalBaseCamp::StorePal(TSubclassOf<AActor> NewPal, int Index)
{
	FPalStoreInventoryData data{};
	data.SpawnClass = NewPal;
	PalStore->StorePal(data, Index);
}

void APalBaseCamp::RemovePal(TSubclassOf<AActor> targetPal, int Index)
{
	FPalStoreInventoryData data{};
	data.SpawnClass = targetPal;
	PalStore->RemovePal(data, Index);
	//PalCommander->RemovePal(targetPal);
}