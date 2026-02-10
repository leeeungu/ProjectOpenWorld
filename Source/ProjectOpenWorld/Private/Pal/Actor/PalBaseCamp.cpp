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
#include "Pal/Subsystem/PalCharacterDataSubsystem.h"
#include "Item/System/ItemDataSubsystem.h"


APalBaseCamp::APalBaseCamp() : Super()
{
	PrimaryActorTick.bCanEverTick = false;

	//Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	//SetRootComponent(Root);
	
	//CampMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	//CampMesh->SetupAttachment(RootComponent);
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
	SpawnComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPoint"));
	SpawnComponent->SetupAttachment(RootComponent);
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
	OtherActor->OnDestroyed.AddUniqueDynamic(this, &APalBaseCamp::WorkDestory);
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

bool APalBaseCamp::SpawnPal(AActor* Spawned)
{
	if (!Spawned)
		return false;

	Spawned->SetActorTransform(SpawnComponent->GetComponentTransform());
	Spawned->SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));
	PalCommander->StorePal(Spawned);
	return true;
}

void APalBaseCamp::DeSpawnPal(AActor* DeSpawned)
{
	if (!DeSpawned)
		return ;
	PalCommander->RemovePal(DeSpawned);
}

void APalBaseCamp::StorePalClass(TSubclassOf<AActor> NewPal, int Index)
{
	//FPalStoreInventoryData data{};
	//data.SpawnActor = NewPal;
	//PalStore->StorePal(data, Index);
}

void APalBaseCamp::RemovePalClass(TSubclassOf<AActor> targetPal, int Index)
{
	/*FPalStoreInventoryData data{};
	data.SpawnActor = GetWorld()->SpawnActor(targetPal);
	PalStore->RemovePal(data, Index);*/
}

void APalBaseCamp::PalDead(AActor* DeadPal)
{
	//PalCommander->RemovePal(DeadPal);
	//PalStore->DeSpawnPal(DeadPal);
}

void APalBaseCamp::WorkDestory(AActor* DeadPal)
{
	PalCommander->UnRegisterWork(DeadPal);
	if(ABaseMonster* Monster = Cast<ABaseMonster>(DeadPal))
	{
		TArray<FPalItemDropData> DropItemList =
		UPalCharacterDataSubsystem::GetDropItemListByCharacterID(Monster->GetMonsterName());
		UE_LOG(LogTemp, Warning, TEXT("APalBaseCamp::Monster Dead Drop Item %s %d"), *Monster->GetMonsterName().ToString(), DropItemList.Num());
		for (const FPalItemDropData& ItemData : DropItemList)
		{
			int Rate = FMath::RandRange(0, 100);
			if (Rate > ItemData.Rate)
				continue;

			UE_LOG(LogTemp, Warning, TEXT("APalBaseCamp::Monster Dead Drop Item Spawn %s "), *ItemData.ItemId.ToString());
			int nCount = FMath::RandRange(ItemData.min, ItemData.Max);
			UItemDataSubsystem::SpawnPalStaticItemVisualActorByName(GetWorld(), ItemData.ItemId, DeadPal->GetActorTransform(), nCount);
		}
	}
}

void APalBaseCamp::NewGenerateWorldEvent(const FGenerateSectionData& SectionData)
{
	PalStore->ShowAllSpawnedPals();
	PalCommander->SetComponentTickEnabled(true);
}

void APalBaseCamp::DelGenerateWorldEvent(const FGenerateSectionData& SectionData)
{
	PalCommander->SetComponentTickEnabled(false);
	PalCommander->StopAllPal();
	PalStore->HideAllSpawnedPals();
}
