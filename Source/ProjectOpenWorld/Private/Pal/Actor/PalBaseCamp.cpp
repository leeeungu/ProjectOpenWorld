#include "Pal/Actor/PalBaseCamp.h"
#include "Pal/Component/PalCommanderComponent.h"
#include "Pal/Component/PalStorageComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Pal/Interface/CommanderManageable.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Creature/Character/BaseMonster.h"
#include "NavigationInvokerComponent.h"

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
	
	NavInvoker = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("NavInvoker"));
}
#if WITH_EDITOR
void APalBaseCamp::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}	
#endif

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
	if (FVector::DistSquared(NewActor->GetActorLocation(), GetActorLocation()) <= CampBounds->GetScaledSphereRadius() * CampBounds->GetScaledSphereRadius())
	{
		PalCommander->RegisterWork(NewActor);
	}
}

void APalBaseCamp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool APalBaseCamp::SpawnPal(int Index, AActor*& Spawned)
{
	Spawned = PalStore->SpawnPal(Index);
	if (Spawned)
	{
		Spawned->OnDestroyed.AddUniqueDynamic(this, &APalBaseCamp::PalDead);
		PalCommander->StorePal(Spawned);
		return true;
	}
	return false;
}

void APalBaseCamp::StorePalClass(TSubclassOf<AActor> NewPal, int Index)
{
	FPalStoreInventoryData data{};
	data.SpawnClass = NewPal;
	PalStore->StorePal(data, Index);
}

void APalBaseCamp::RemovePalClass(TSubclassOf<AActor> targetPal, int Index)
{
	FPalStoreInventoryData data{};
	data.SpawnClass = targetPal;
	PalStore->RemovePal(data, Index);
}

void APalBaseCamp::PalDead(AActor* DeadPal)
{
	PalCommander->RemovePal(DeadPal);
	PalStore->DeSpawnPal(DeadPal);
}

void APalBaseCamp::NewGenerateWorldEvent(const FGenerateSectionData& SectionData)
{
	UE_LOG(LogTemp, Error, TEXT("TODO :: ALL PAL START"));
}

void APalBaseCamp::DelGenerateWorldEvent(const FGenerateSectionData& SectionData)
{
	UE_LOG(LogTemp, Error, TEXT("TODO :: ALL PAL STOP, COOMMAND NOT RESET이어야 하나?"));
}
