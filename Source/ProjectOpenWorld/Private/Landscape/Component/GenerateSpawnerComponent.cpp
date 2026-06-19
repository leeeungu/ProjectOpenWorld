#include "Landscape/Component/GenerateSpawnerComponent.h"
#include "Landscape/Component/GeneratorSectionComponent.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Pal/Actor/PalMonsterSpawner.h"
#include "Components/BrushComponent.h"
#include "NavigationSystem.h"

UWorld* FSpawnerQuadNode::TestWorld = nullptr;

// ===== FSpawnerQuadNode =====
void FSpawnerQuadNode::Subdivide(int32 InMaxDepth)
{
    if (Depth >= InMaxDepth)
    { 
        bLeaf = true; 
//#if WITH_EDITOR
//        const float Q = HalfExtent;
//        DrawDebugBox(TestWorld, FVector(Center.X, Center.Y, 0), FVector(Q, Q, Q), FQuat{}, FColor::Yellow, true);
//#endif
        return; 
    }
    const float Q = HalfExtent * 0.5f;
    const FVector2D Off[4] = { {-Q,-Q}, {+Q,-Q}, {-Q,+Q}, {+Q,+Q} };
    for (int32 i = 0; i < 4; ++i)
    {
        Children[i] = MakeUnique<FSpawnerQuadNode>();
        Children[i]->Center = Center + Off[i];
        Children[i]->Depth = Depth + 1;
        Children[i]->HalfExtent = Q;
        Children[i]->Subdivide(InMaxDepth);
    }
}

FSpawnerQuadNode* FSpawnerQuadNode::FindLeaf(const FVector2D& P)
{
    if (bLeaf) 
        return this;
    const int32 ix = (P.X >= Center.X) ? 1 : 0;
    const int32 iy = (P.Y >= Center.Y) ? 1 : 0;
    FSpawnerQuadNode* Child = Children[iy * 2 + ix].Get();   // Off 순서와 일치
    return Child ? Child->FindLeaf(P) : this;
}

// ===== 라이프사이클 =====
UGenerateSpawnerComponent::UGenerateSpawnerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UGenerateSpawnerComponent::BeginPlay()
{
    FSpawnerQuadNode::TestWorld = GetWorld();
    Super::BeginPlay();
    EnsureRowsCached();
}

void UGenerateSpawnerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().PauseTimer(PollTimerHandle);
        GetWorld()->GetTimerManager().ClearTimer(PollTimerHandle);
    }
    Super::EndPlay(EndPlayReason);
}

void UGenerateSpawnerComponent::StartGenerateWorld(bool bEditor)
{
    if (!GetWorld() || !GetWorld()->HasBegunPlay())
        return;

    Super::StartGenerateWorld(bEditor);
    EnsureRowsCached();
    StartPollTimer();
}

void UGenerateSpawnerComponent::NewGenerateWorld(const FGenerateSectionData& SectionData)
{
    if (!GetWorld() || !GetWorld()->HasBegunPlay())
        return;
    if (!SectionData.Vertices || SectionData.Vertices->Num() <= 1)
        return;

    BuildCandidatesForSection(SectionData);

    // 막 들어온 섹션이 이미 플레이어 반경 안이면 즉시 활성 (플레이어가 안 움직여도)
    if (TUniquePtr<FSpawnerQuadNode>* Found = SectionTrees.Find(SectionData.SectionID))
    {
        const FVector P = GetPlayerLocation();
        ReconcileNode(Found->Get(), FVector2D(P.X, P.Y));
    }
}

void UGenerateSpawnerComponent::DelGenerateWorld(const FGenerateSectionData& SectionData)
{
    if (!GetWorld() || !GetWorld()->HasBegunPlay())
        return;

    if (TUniquePtr<FSpawnerQuadNode>* Found = SectionTrees.Find(SectionData.SectionID))
    {
        DeactivateNode(Found->Get());          // 활성 스포너 풀로 회수
        SectionTrees.Remove(SectionData.SectionID);
    }
}

// ===== 셋업 =====
void UGenerateSpawnerComponent::EnsureRowsCached()
{
    if (bRowsCached) 
        return;
    if (!HabitatDataTable) 
        return;

    CachedRows.Reset();
    TArray<FPalSpawnHabitatRow*> Rows;
    HabitatDataTable->GetAllRows<FPalSpawnHabitatRow>(TEXT("UGenerateSpawnerComponent::EnsureRowsCached"), Rows);
    CachedRows.Reserve(Rows.Num());
    for (const FPalSpawnHabitatRow* R : Rows)
        if (R) CachedRows.Add(R);

    bRowsCached = true;
}

void UGenerateSpawnerComponent::StartPollTimer()
{
    if (GetWorld()->GetTimerManager().IsTimerActive(PollTimerHandle))
        return;
    if (!GeneratorSectionComponent) 
        return;

    const FVector SectionSize = GeneratorSectionComponent->GetSectionSize();
    if (SectionSize.X <= 0.f)
        return;

    LeafExtent = SectionSize.X / static_cast<float>(1 << MaxDepth);   // 섹션변 / 2^Depth
    if (LeafExtent <= 0.f)
        return;

    GetWorld()->GetTimerManager().SetTimer(PollTimerHandle, this, &UGenerateSpawnerComponent::OnPollTimer, PollInterval, true);
}

void UGenerateSpawnerComponent::OnPollTimer()
{
    const FVector P = GetPlayerLocation();

    // 리프 격자가 아니라 실제 이동 거리로 게이트 (갱신 주기를 리프 크기와 분리)
    if (FVector::DistSquared2D(P, LastReconcilePos) < FMath::Square(ReconcileMoveThreshold))
        return;
    LastReconcilePos = P;

    // ===== 플레이어 거리 기반 활성 (쿼드트리 range query) =====
    const FVector2D P2(P.X, P.Y);
    for (TPair<FIntPoint, TUniquePtr<FSpawnerQuadNode>>& Pair : SectionTrees)
        ReconcileNode(Pair.Value.Get(), P2);

    if (bDebugDraw && GetWorld())
        DrawDebugSphere(GetWorld(), P, ActivationRadius, 24, FColor::Cyan, false, PollInterval, 0, 30.f);
}

// ===== 후보 적재 (habitat 룰, 원본 동일) =====
void UGenerateSpawnerComponent::BuildCandidatesForSection(const FGenerateSectionData& SectionData)
{
    if (CachedRows.IsEmpty() || SectionTrees.Contains(SectionData.SectionID))
        return;
    if (!SectionData.Vertices || !SectionData.Normals || !GeneratorSectionComponent)
        return;

    const FVector SectionSize = GeneratorSectionComponent->GetSectionSize();
    if (SectionSize.X <= 0.f)
        return;

    TUniquePtr<FSpawnerQuadNode> Root = MakeUnique<FSpawnerQuadNode>();
    Root->Center = FVector2D((SectionData.SectionID.X + 0.5f) * SectionSize.X,
        (SectionData.SectionID.Y + 0.5f) * SectionSize.Y);
    Root->HalfExtent = SectionSize.X * 0.5f;
    Root->Depth = 0;
    Root->Subdivide(MaxDepth);

    const TArray<FVector>& Verts = *SectionData.Vertices;
    const TArray<FVector>& Normals = *SectionData.Normals;

    const uint32 Seed = HashCombine(GetTypeHash(SectionData.SectionID.X),
        GetTypeHash(SectionData.SectionID.Y));
    FRandomStream RS(static_cast<int32>(Seed));

    for (const FPalSpawnHabitatRow* Row : CachedRows)
    {
        if (Row && Row->SpawnerDt.LoadSynchronous())
        {
            const int32 IntPart = FMath::FloorToInt(Row->SectionDensity);
            const float FracPart = Row->SectionDensity - IntPart;
            const int32 Tries = IntPart + (RS.FRand() < FracPart ? 1 : 0);

            for (int32 i = 0; i < Tries; ++i)
            {
                const int32 Idx = RS.RandRange(0, Verts.Num() - 1);
                const FVector& V = Verts[Idx];
                const FVector& N = Normals.IsValidIndex(Idx) ? Normals[Idx] : FVector::UpVector;

                if (V.Z >= Row->MinHeight && V.Z <= Row->MaxHeight)
                {
                    const float SlopeDeg = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(N.Z, -1.f, 1.f)));
                    if (SlopeDeg <= Row->MaxSlope)
                    {
                        FSpawnerSpec Spec;
                        Spec.Location = V;
                        Spec.Habitat = Row;
                        if (FSpawnerQuadNode* Leaf = Root->FindLeaf(FVector2D(V.X, V.Y)))
                            Leaf->Specs.Add(Spec);
                    }
                }
            }
        }
    }

    SectionTrees.Add(SectionData.SectionID, MoveTemp(Root));
}

void UGenerateSpawnerComponent::ReconcileNode(FSpawnerQuadNode* Node, const FVector2D& P)
{
    if (!Node) 
        return;

    if (!IsNodeWithinRadius(Node, P))
    {
        DeactivateNode(Node);
        return;
    }
    if (Node->bLeaf)
    {
        ActivateNode(Node);
        return;
    }
    for (int32 i = 0; i < 4; ++i)
        ReconcileNode(Node->Children[i].Get(), P);
}

bool UGenerateSpawnerComponent::IsNodeWithinRadius(const FSpawnerQuadNode* Node, const FVector2D& P) const
{
    const double dx = FMath::Max(0.f, FMath::Abs(P.X - Node->Center.X) - Node->HalfExtent);
    const double dy = FMath::Max(0.f, FMath::Abs(P.Y - Node->Center.Y) - Node->HalfExtent);
    double Sqr = (double)ActivationRadius * (double)ActivationRadius;
    return (dx * dx + dy * dy) <= Sqr;   // 원 vs 노드 AABB
}

// ===== 노드 활성/비활성 =====
void UGenerateSpawnerComponent::ActivateNode(FSpawnerQuadNode* Node)
{
    if (!Node) 
        return;
    if (Node->bLeaf)
    {
        if (Node->bActive) 
            return;
        Node->bActive = true;
        // 리프 중심에서 아래로 트레이스해 지형 표면 Z를 찾는다
        const FVector2D C = Node->Center;
        const FVector Start(C.X, C.Y, TraceStartZ);
        const FVector End(C.X, C.Y, TraceEndZ);
        FHitResult Hit;

        GetWorld()->LineTraceSingleByChannel(Hit, Start, End, TerrainChannel, FCollisionQueryParams::DefaultQueryParam);
        //return;   // 지형 못 맞힘(콜리전 아직?) → bActive 안 켜고 다음 reconcile에서 재시도
        const FVector Loc(C.X, C.Y, Hit.ImpactPoint.Z );
        // 스폰 시 ANavMeshBoundsVolume이 nav 시스템에 자동 등록 → 해당 영역 갱신
        FTransform Trans{};
        Trans.SetLocation(Loc);
        const float Q = Node->HalfExtent;
        FVector Size = FVector::One() * Q;
        Size.Z = ZSize;
        Trans.SetScale3D(Size);
        Node->Volume = GetWorld()->SpawnActor<ANavMeshBoundsVolume>(NavBoundClass, Trans);
//#if WITH_EDITOR
//        DrawDebugBox(GetWorld(), Loc, Size, FQuat{}, FColor::Emerald, false, 5.0f);
//#endif
        if (Node->Volume)
        {
            Node->Volume->GetBrushComponent()->Bounds.BoxExtent = Size;
            UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
            if (GIsEditor && NavSys)
            {
                NavSys->OnNavigationBoundsUpdated(Node->Volume);
            }
        }

        Node->ActiveSpawners.Reserve(Node->Specs.Num());
        for (const FSpawnerSpec& Spec : Node->Specs)
        {
            if (!Spec.Habitat || !Spec.Habitat->SpawnerDt.IsValid()) continue;
            if (APalMonsterSpawner* S = AcquireSpawner(Spec.Habitat->SpawnerDt.LoadSynchronous(), Spec.Location))
                Node->ActiveSpawners.Add(S);
        }
        return;
    }
    for (int32 i = 0; i < 4; ++i)
        ActivateNode(Node->Children[i].Get());
}

void UGenerateSpawnerComponent::DeactivateNode(FSpawnerQuadNode* Node)
{
    if (!Node) 
        return;
    if (Node->bLeaf)
    {
        if (!Node->bActive)
            return;
        for (TObjectPtr<APalMonsterSpawner>& S : Node->ActiveSpawners)
        {
            if (S)
                ReleaseSpawner(S);
        }

        Node->ActiveSpawners.Reset();
        Node->bActive = false;
        if (Node->Volume)
        {
//#if WITH_EDITOR
//            DrawDebugBox(GetWorld(), Node->Volume->GetActorLocation(), Node->Volume->GetActorScale3D(), FQuat{}, FColor::Red, false, 5.0f);
//#endif
            Node->Volume->Destroy();
        }
        return;
    }
    for (int32 i = 0; i < 4; ++i)
        DeactivateNode(Node->Children[i].Get());
}

// ===== 스포너 풀 =====
APalMonsterSpawner* UGenerateSpawnerComponent::AcquireSpawner(UDataTable* Dt, const FVector& Loc)
{
    if (bUseActorPool)
    {
        while (!ActorPool.IsEmpty())
        {
            APalMonsterSpawner* S = ActorPool.Pop().Get();
            if (!IsValid(S)) continue;

            S->GetRootComponent()->SetMobility(EComponentMobility::Movable);
            S->SetActorLocationAndRotation(Loc, FRotator::ZeroRotator, false, nullptr, ETeleportType::TeleportPhysics);
            S->GetRootComponent()->SetMobility(EComponentMobility::Static);
            S->SetActorHiddenInGame(false);
            S->SetActorEnableCollision(true);
            S->SetActorTickEnabled(true);
            S->Initialize(Dt);
            return S;
        }
    }

    const FTransform T(FRotator::ZeroRotator, Loc);
    APalMonsterSpawner* S = GetWorld()->SpawnActorDeferred<APalMonsterSpawner>(APalMonsterSpawner::StaticClass(), T, GetOwner(), nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
    if (S)
    {
        S->Initialize(Dt);
        S->FinishSpawning(T);
    }
    return S;
}

void UGenerateSpawnerComponent::ReleaseSpawner(APalMonsterSpawner* S)
{
    if (!IsValid(S)) return;

    S->Teardown();

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

FVector UGenerateSpawnerComponent::GetPlayerLocation() const
{
    if (ACharacter* PC = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
        return PC->GetActorLocation();
    return FVector::ZeroVector;
}