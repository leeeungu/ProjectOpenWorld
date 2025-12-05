#include "Pal/Actor/PalBaseCamp.h"
#include "Pal/Component/PalCommanderComponent.h"
#include "Pal/Component/PalStorageComponent.h"
#include "Components/StaticMeshComponent.h"

APalBaseCamp::APalBaseCamp()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	CampMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	CampMesh->SetupAttachment(RootComponent);
	PalStore = CreateDefaultSubobject<UPalStorageComponent>(TEXT("PalStorage"));
	PalCommander = CreateDefaultSubobject<UPalCommanderComponent>(TEXT("PalCommander"));
}

void APalBaseCamp::BeginPlay()
{
	Super::BeginPlay();
}

void APalBaseCamp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

