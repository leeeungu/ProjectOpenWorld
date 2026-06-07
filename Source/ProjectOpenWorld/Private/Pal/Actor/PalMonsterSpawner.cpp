#include "Pal/Actor/PalMonsterSpawner.h"
#include "Components/SceneComponent.h"
#include "NavigationInvokerComponent.h"
#include "NavigationSystem.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Pal/Character/PalBaseMonster.h"

#include "Components/BrushComponent.h"

APalMonsterSpawner::APalMonsterSpawner() : Super{}
{
    PrimaryActorTick.bCanEverTick = false;
    //RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    NavigationInvokerComp = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("NavigationInvokerComp"));
    NavigationInvokerComp->SetComponentTickEnabled(false);
}

void APalMonsterSpawner::BeginPlay()
{
    Super::BeginPlay();
   
}

void APalMonsterSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Teardown();
    Super::EndPlay(EndPlayReason);
}

// ──────────────────────────────────────────────────────────────

void APalMonsterSpawner::Initialize(UDataTable* InMonsterDt)
{
    Teardown();                       // reuse 안전

    MonsterDt = InMonsterDt;
    if (!MonsterDt)
        return;

    TArray<FPalMonsterData*> Rows;
    MonsterDt->GetAllRows<FPalMonsterData>(TEXT("APalMonsterSpawner::Initialize"), Rows);
    if (Rows.IsEmpty() || !Rows[0])
    {
        UE_LOG(LogTemp, Warning, TEXT("APalMonsterSpawner::Initialize - empty DT %s"),
            *GetNameSafe(InMonsterDt));
        return;
    }

    MonsterData = *Rows[0];           // 단일 펠 DT 가정 — 첫 row 만
    TargetNum = FMath::RandRange(MonsterData.NumMin, MonsterData.NumMax);

    StartSpawnLoop();
}

void APalMonsterSpawner::Teardown()
{
    StopSpawnLoop();

   /* if (!AliveMonsters.IsEmpty())
    {
        for (TObjectPtr<APalBaseMonster>& M : AliveMonsters)
        {
            if (IsValid(M))
            {
                M->Destroy();
                M = nullptr;
            }
        }
    }*/
    AliveMonsters.Reset();
    TargetNum = 0;
}

// ──────────────────────────────────────────────────────────────

void APalMonsterSpawner::StartSpawnLoop()
{
    if (!GetWorld()) 
        return;
    GetBrushComponent()->Bounds.BoxExtent = FVector(StaticRadius, StaticRadius, StaticRadius);
    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (GIsEditor && NavSys)
    {
        NavSys->OnNavigationBoundsUpdated(this);
    }
    FActorSpawnParameters Params;
    Params.Owner = this;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    const float Interval = FMath::Max(MonsterData.SpawnTime, 1.f);
    GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &APalMonsterSpawner::OnSpawnTick, Interval, true, Interval);

    FVector SpawnLocation = GetActorLocation();
    SpawnLocation.Z += 100.f;
    APalBaseMonster* Spawned = GetWorld()->SpawnActor<APalBaseMonster>(MonsterData.MonsterClass, SpawnLocation, GetActorRotation(), Params);
    if (!Spawned) 
        return;

    Spawned->OnDestroyed.AddUniqueDynamic(this, &APalMonsterSpawner::OnMonsterDead);
    AliveMonsters.Add(Spawned);

    const int32 Lv = FMath::RandRange(MonsterData.LvMin, MonsterData.LvMax);
    Spawned->InitializeLevel(Lv, GetLevelData(Lv));

}

void APalMonsterSpawner::StopSpawnLoop()
{
    if (GetWorld())
        GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
}

void APalMonsterSpawner::OnSpawnTick()
{
    if (AliveMonsters.Num() >= TargetNum)
        return;
    if (!*MonsterData.MonsterClass)
        return;

    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (!NavSys)
        return;
        FNavLocation SpawnLoc{};
    if (!NavSys->GetRandomReachablePointInRadius(GetActorLocation(), SpawnRadius, SpawnLoc))
    {
        UE_LOG(LogTemp, Warning, TEXT("APalMonsterSpawner::OnSpawnTick  No Location %s"), *SpawnLoc.Location.ToString());
        return;
    }

    SpawnLoc.Location.Z += 100.f;     // 캐릭터 캡슐 띄우기

    FActorSpawnParameters Params;
    Params.Owner = this;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    APalBaseMonster* Spawned = GetWorld()->SpawnActor<APalBaseMonster>(
        MonsterData.MonsterClass, SpawnLoc.Location, GetActorRotation(), Params);
    if (!Spawned) return;

    Spawned->OnDestroyed.AddUniqueDynamic(this, &APalMonsterSpawner::OnMonsterDead);
    AliveMonsters.Add(Spawned);

    const int32 Lv = FMath::RandRange(MonsterData.LvMin, MonsterData.LvMax);
    Spawned->InitializeLevel(Lv, GetLevelData(Lv));
}

void APalMonsterSpawner::OnMonsterDead(AActor* DeadPal)
{
    if (APalBaseMonster* M = Cast<APalBaseMonster>(DeadPal))
        AliveMonsters.Remove(M);
}

FPalMonsterLevelData APalMonsterSpawner::GetLevelData(int32 Level) const
{
    if (!MonsterData.LevelDataTable) return {};

    TArray<FPalMonsterLevelData*> Rows;
    MonsterData.LevelDataTable->GetAllRows<FPalMonsterLevelData>(TEXT(""), Rows);
    if (Rows.IsValidIndex(Level - 1))   return *Rows[Level - 1];
    if (Rows.IsValidIndex(0))           return *Rows[0];
    return {};
}