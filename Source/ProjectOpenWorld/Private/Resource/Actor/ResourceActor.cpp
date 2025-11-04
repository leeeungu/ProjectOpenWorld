#include "Resource/Actor/ResourceActor.h"
#include "Item/Actor/ItemActor.h"
#include "Components/StaticMeshComponent.h"

AResourceActor::AResourceActor()
{
	PrimaryActorTick.bCanEverTick = true;
	ResourceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(ResourceMesh);
}

void AResourceActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AResourceActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (onExtractEnd.IsBound())
		onExtractEnd.Broadcast();
}

void AResourceActor::SpawnRandomItem()
{
	if (ItemData.Num() == 0)
		return;
	FActorSpawnParameters Param{};
	Param.Owner = this;
	Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	FVector Location = GetActorLocation();
	AItemActor* Item = Cast< AItemActor>(GetWorld()->SpawnActor(AItemActor::StaticClass(), &Location, 0, Param));
	int idx = rand() % ItemData.Num();
	Item->Init(ItemData[idx], ItemData.Num() + 2);
	ExtractCount--;
	if (ExtractCount <= 0)
	{
		Destroy();
	}
}


