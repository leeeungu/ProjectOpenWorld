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
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	CampMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	CampMesh->SetupAttachment(RootComponent);
	PalStore = CreateDefaultSubobject<UPalStorageComponent>(TEXT("PalStorage"));
	PalCommander = CreateDefaultSubobject<UPalCommanderComponent>(TEXT("PalCommander"));
	CampBounds = CreateDefaultSubobject <USphereComponent >(TEXT("CampBound"));
	CampBounds->SetupAttachment(RootComponent);
	CampBounds->SetSphereRadius(1200.0f);
	CampBounds->OnComponentBeginOverlap.Clear();
	CampBounds->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel3);
	CampBounds->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	CampBounds->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CampBounds->SetGenerateOverlapEvents(true);

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

	CampBounds->OnComponentBeginOverlap.AddUniqueDynamic(this, &APalBaseCamp::OnCampBeginOverlap);
	CampBounds->OnComponentEndOverlap.AddUniqueDynamic(this, &APalBaseCamp::OnCampEndOverlap);
	//FOnActorSpawned::FDelegate del{};
	//del.BindUObject(this, &APalBaseCamp::CommandActorSpawned);
	//if (GetWorld())
	//	GetWorld()->AddOnActorSpawnedHandler(del);

	TArray<TEnumAsByte<EObjectTypeQuery>> arQuery{};
	arQuery.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	arQuery.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	arQuery.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(GetOwner());
	TArray<FHitResult> arHitted{};
	bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetWorld(),
		GetActorLocation(), GetActorLocation(), CampBounds->GetScaledSphereRadius(), arQuery,
		false, IgnoreActors, EDrawDebugTrace::Type::None, arHitted, true);
	for(FHitResult& hit : arHitted)
	{
		PalCommander->RegisterWork(hit.GetActor());
	}
}

void APalBaseCamp::OnCampBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("BaseCamp Begin Overlap %s"), *OtherActor->GetName());
	if (!OtherActor || !PalCommander)
		return;
	PalCommander->RegisterWork(OtherActor);
}

void APalBaseCamp::OnCampEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("BaseCamp End Overlap %s"), *OtherActor->GetName());
	if (!OtherActor || !PalCommander)
		return;
	PalCommander->UnRegisterWork(OtherActor);
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
	PalStore->ShowAllSpawnedPals();
	PalCommander->SetComponentTickEnabled(true);
	//UE_LOG(LogTemp, Error, TEXT("TODO :: ALL PAL START"));
}

void APalBaseCamp::DelGenerateWorldEvent(const FGenerateSectionData& SectionData)
{
	PalCommander->SetComponentTickEnabled(false);
	PalCommander->StopAllPal();
	PalStore->HideAllSpawnedPals();
	//UE_LOG(LogTemp, Error, TEXT("TODO :: ALL PAL STOP, COOMMAND NOT RESET이어야 하나?"));
}
