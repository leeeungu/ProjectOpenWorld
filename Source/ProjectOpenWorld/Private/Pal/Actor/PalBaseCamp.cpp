#include "Pal/Actor/PalBaseCamp.h"
#include "Pal/Component/PalCommanderComponent.h"
#include "Pal/Component/PalStorageComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Pal/Interface/CommanderManageable.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Creature/Character/BaseMonster.h"

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
	FOnActorSpawned::FDelegate del{};
	del.BindUObject(this, &APalBaseCamp::CommandActorSpawned);
	if (GetWorld())
		GetWorld()->AddOnActorSpawnedHandler(del);

	TArray<TEnumAsByte<EObjectTypeQuery>> arQuery{};
	arQuery.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	arQuery.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	arQuery.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));
	arQuery.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel1));

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(GetOwner());
	TArray<FHitResult> arHitted{};
	bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetWorld(),
		GetActorLocation(), GetActorLocation(), CampBounds->GetScaledSphereRadius(), arQuery,
		false, IgnoreActors, EDrawDebugTrace::Type::None, arHitted, true);

	for(FHitResult& hit : arHitted)
	{
		CommandActorSpawned(hit.GetActor());
	}
}

void APalBaseCamp::CommandActorSpawned(AActor* NewActor)
{
	if (!NewActor || !PalCommander)
		return;
	//if (NewActor->StaticClass() == ABaseMonster::StaticClass())
	//{
	//	UE_LOG()
	//}
	PalCommander->RegisterWork(NewActor);
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