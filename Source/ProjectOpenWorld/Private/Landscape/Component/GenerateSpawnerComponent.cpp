#include "Landscape/Component/GenerateSpawnerComponent.h"
#include "Landscape/Component/GeneratorSectionComponent.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Pal/Actor/PalMonsterSpawner.h"

UGenerateSpawnerComponent::UGenerateSpawnerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UGenerateSpawnerComponent::BeginPlay()
{
    Super::BeginPlay();
    EnsureRowsCached();
}

void UGenerateSpawnerComponent::StartGenerateWorld(bool bEditor)
{
    if (!GetWorld()->HasBegunPlay())
        return;

    Super::StartGenerateWorld(bEditor);
    EnsureRowsCached();   // 에디터 프리뷰 경로 보장
}

void UGenerateSpawnerComponent::NewGenerateWorld(const FGenerateSectionData& SectionData)
{
    if (!SectionData.Vertices || SectionData.Vertices->Num() <= 1 || !GetWorld()->HasBegunPlay())
        return;
    BuildCandidatesForSection(SectionData);
}

void UGenerateSpawnerComponent::DelGenerateWorld(const FGenerateSectionData& SectionData)
{
    if (!GetWorld()->HasBegunPlay())
        return;

    // 6x6 에서 빠진 섹션: 활성이었으면 디스폰 + 후보 제거
    DeactivateSection(SectionData.SectionID);
    SectionSpawnerMap.Remove(SectionData.SectionID);
    ActiveSectionSet.Remove(SectionData.SectionID);
}

void UGenerateSpawnerComponent::FinishGenerateWorld()
{
    if (!GetWorld()->HasBegunPlay())
        return;
    Super::FinishGenerateWorld();
    UpdateActiveRing();
}

void UGenerateSpawnerComponent::EnsureRowsCached()
{
    if (bRowsCached) return;
    if (!HabitatDataTable) return;

    CachedRows.Reset();
    TArray<FPalSpawnHabitatRow*> Rows;
    HabitatDataTable->GetAllRows<FPalSpawnHabitatRow>(TEXT("UGenerateSpawnerComponent::EnsureRowsCached"), Rows);
    CachedRows.Reserve(Rows.Num());
    for (const FPalSpawnHabitatRow* R : Rows)
        if (R) CachedRows.Add(R);

    bRowsCached = true;
}

void UGenerateSpawnerComponent::BuildCandidatesForSection(const FGenerateSectionData& SectionData)
{
    if (CachedRows.IsEmpty()) 
        return;

    FSpawnerSectionData& Section = SectionSpawnerMap.FindOrAdd(SectionData.SectionID);
    if (!Section.Candidates.IsEmpty())
        return;

    const TArray<FVector>& Verts = *SectionData.Vertices;
    const TArray<FVector>& Normals = *SectionData.Normals;

    const uint32 Seed = HashCombine(GetTypeHash(SectionData.SectionID.X),
        GetTypeHash(SectionData.SectionID.Y));
    FRandomStream RS(static_cast<int32>(Seed));

    for (const FPalSpawnHabitatRow* Row : CachedRows)
    {
        if (!Row) continue;

        // 소프트 → 하드 (첫 호출만 비용, 이후 캐시)
        UDataTable* DT = Row->SpawnerDt.LoadSynchronous();
        if (!DT) continue;

        const int32 IntPart = FMath::FloorToInt(Row->SectionDensity);
        const float FracPart = Row->SectionDensity - IntPart;
        const int32 Tries = IntPart + (RS.FRand() < FracPart ? 1 : 0);

        for (int32 i = 0; i < Tries; i++)
        {
            const int32 Idx = RS.RandRange(0, Verts.Num() - 1);
            const FVector& V = Verts[Idx];
            const FVector& N = Normals.IsValidIndex(Idx) ? Normals[Idx] : FVector::UpVector;

            if (V.Z < Row->MinHeight || V.Z > Row->MaxHeight) continue;

            const float SlopeDeg = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(N.Z, -1.f, 1.f)));
            if (SlopeDeg > Row->MaxSlope) continue;

            FSpawnerCandidate C;
            C.Location = V;
            C.SpawnerDt = Row;
            Section.Candidates.Add(C);
        }
    }
}

void UGenerateSpawnerComponent::UpdateActiveRing()
{
    // 에디터 프리뷰에선 캐릭터 활성 안 함 (후보만 보임)
    if (!GeneratorSectionComponent)
        return;

    const FIntPoint Player = GeneratorSectionComponent->GetPlayerSection();
    const int32 R = ActiveSectionRadius;

    TSet<FIntPoint> NewRing;
    NewRing.Reserve((2 * R + 1) * (2 * R + 1));
    for (int32 dy = -R; dy <= R; dy++)
        for (int32 dx = -R; dx <= R; dx++)
            NewRing.Add(FIntPoint(Player.X + dx, Player.Y + dy));

    // diff: 빠진 곳은 비활성, 새로 들어온 곳은 활성
    for (const FIntPoint& Old : ActiveSectionSet)
        if (!NewRing.Contains(Old))
            DeactivateSection(Old);

    for (const FIntPoint& New : NewRing)
        if (!ActiveSectionSet.Contains(New))
            ActivateSection(New);

    ActiveSectionSet = MoveTemp(NewRing);

    if (bDebugDraw && GetWorld() && GeneratorSectionComponent)
    {
        const FVector SectionSize = GeneratorSectionComponent->GetSectionSize();
        for (const FIntPoint& ID : ActiveSectionSet)
        {
            const FVector Center((ID.X + 0.5f) * SectionSize.X,
                (ID.Y + 0.5f) * SectionSize.Y,
                5000.f);
            DrawDebugBox(GetWorld(), Center, SectionSize * 0.5f, FColor::Cyan, false, 5.f, 0, 50.f);
        }
    }
}

void UGenerateSpawnerComponent::ActivateSection(const FIntPoint& ID)
{
    FSpawnerSectionData* Data = SectionSpawnerMap.Find(ID);
    if (!Data || Data->bActive) return;
    Data->bActive = true;

    for (FSpawnerCandidate& C : Data->Candidates)
    {
        if (!C.ActiveSpawner && C.SpawnerDt && C.SpawnerDt->SpawnerDt.IsValid())
        {
            C.ActiveSpawner = AcquireSpawner(C.SpawnerDt->SpawnerDt.LoadSynchronous(), C.Location);
        }
    }
}

void UGenerateSpawnerComponent::DeactivateSection(const FIntPoint& ID)
{
    FSpawnerSectionData* Data = SectionSpawnerMap.Find(ID);
    if (!Data || !Data->bActive) return;
    Data->bActive = false;

    for (FSpawnerCandidate& C : Data->Candidates)
    {
        if (C.ActiveSpawner)
        {
            ReleaseSpawner(C.ActiveSpawner);
            C.ActiveSpawner = nullptr;
        }
    }
}

APalMonsterSpawner* UGenerateSpawnerComponent::AcquireSpawner(UDataTable* Dt, const FVector& Loc)
{
    if (bUseActorPool)
    {
        while (!ActorPool.IsEmpty())
        {
            APalMonsterSpawner* S = ActorPool.Pop().Get();
            if (!IsValid(S))
                continue;

            S->GetRootComponent()->SetMobility(EComponentMobility::Movable);
            S->SetActorLocationAndRotation(Loc, FRotator::ZeroRotator, false, nullptr, ETeleportType::TeleportPhysics);
            S->GetRootComponent()->SetMobility(EComponentMobility::Static);
            S->SetActorHiddenInGame(false);
            S->SetActorEnableCollision(true);
            S->SetActorTickEnabled(true);
            S->Initialize(Dt);                      // ★ APalMonsterSpawner 에 필요한 API (아래 설명)
            return S;
        }
    }

    FTransform T(FRotator::ZeroRotator, Loc);

    APalMonsterSpawner* S = GetWorld()->SpawnActorDeferred<APalMonsterSpawner>(
        APalMonsterSpawner::StaticClass(), T, GetOwner(), nullptr,
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
    if (S)
    {
        S->Initialize(Dt);                          // ★ FinishSpawning 전에 DT 주입
        S->FinishSpawning(T);
    }
    return S;
}

void UGenerateSpawnerComponent::ReleaseSpawner(APalMonsterSpawner* S)
{
    if (!IsValid(S)) 
        return;

    if (bUseActorPool && ActorPool.Num() < PoolCap)
    {
        S->SetActorHiddenInGame(true);
        S->SetActorEnableCollision(false);
        S->SetActorTickEnabled(false);
        ActorPool.Add(S);
        return;
    }
    S->Destroy();
}