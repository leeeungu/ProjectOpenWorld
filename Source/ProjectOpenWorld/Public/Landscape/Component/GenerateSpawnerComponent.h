#pragma once

#include "CoreMinimal.h"
#include "Landscape/Component/GenerateWorldComponent.h"
#include "Pal/DataTable/PalSpawnHabitatRow.h"
#include "GenerateSpawnerComponent.generated.h"

class APalMonsterSpawner;
class ANavMeshBoundsVolume;

// habitat row 포인터를 들고 있는 한 스폰 후보(명세서)
USTRUCT()
struct FSpawnerSpec
{
    GENERATED_USTRUCT_BODY()
    FVector Location = FVector::ZeroVector;
    const FPalSpawnHabitatRow* Habitat = nullptr;
};

// 섹션 1개당 1트리. 리프가 Specs + 활성 스포너를 관리 (UObject 아님)
struct FSpawnerQuadNode
{
#if WITH_EDITORONLY_DATA
    static UWorld* TestWorld;
#endif
    FVector2D Center{};
    double     HalfExtent = 0.0;
    int32     Depth = 0;
    bool      bLeaf = false;
    TUniquePtr<FSpawnerQuadNode> Children[4]{};

    TArray<FSpawnerSpec> Specs{};
    UPROPERTY()
    TObjectPtr<ANavMeshBoundsVolume> Volume{};
    UPROPERTY()
    TArray<TObjectPtr<APalMonsterSpawner>> ActiveSpawners{};
    bool bActive = false;

    void Subdivide(int32 InMaxDepth);
    FSpawnerQuadNode* FindLeaf(const FVector2D& P);
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTOPENWORLD_API UGenerateSpawnerComponent : public UGenerateWorldComponent
{
    GENERATED_BODY()
    UPROPERTY()
    TArray<TObjectPtr<APalMonsterSpawner>> ActorPool{};

    UPROPERTY(EditAnywhere, Category = "Landscape Settings|Spawner", meta = (ClampMin = 0, EditCondition = "bUseActorPool"))
    int32 PoolCap = 128;
public:
    UGenerateSpawnerComponent();
    virtual void StartGenerateWorld(bool bEditor = false) override;
    virtual void NewGenerateWorld(const FGenerateSectionData& SectionData) override;
    virtual void DelGenerateWorld(const FGenerateSectionData& SectionData) override;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(EditAnywhere, Category = "Landscape Settings|Spawner")
    TObjectPtr<UDataTable> HabitatDataTable = nullptr;

    // 섹션 분할 깊이 (2 => 16 리프)
    UPROPERTY(EditAnywhere, Category = "Landscape Settings|Spawner", meta = (ClampMin = 1, ClampMax = 10))
    int32 MaxDepth = 2;

    // 플레이어로부터 이 거리(cm) 안의 리프만 스폰
    UPROPERTY(EditAnywhere, Category = "Landscape Settings|Spawner", meta = (ClampMin = 0))
    float ActivationRadius = 30000.f;

    // 플레이어 위치 폴링 주기(초)
    UPROPERTY(EditAnywhere, Category = "Landscape Settings|Spawner", meta = (ClampMin = 0.05))
    float PollInterval = 0.4f;

    UPROPERTY(EditAnywhere, Category = "Landscape Settings|Spawner")
    bool bUseActorPool = true;
    
    UPROPERTY(EditAnywhere, Category = "Landscape Settings|Spawner", meta = (ClampMin = "0.0"))
    float ReconcileMoveThreshold{ 500.f };

    UPROPERTY(EditAnywhere, Category = "Landscape Settings|Spawner|Debug")
    bool bDebugDraw = false;

    // 리프 크기로 미리 만든 NavMeshBoundsVolume BP (큐브 XY = 리프 한 변, Z는 지형 높이 덮게)
    UPROPERTY(EditAnywhere, Category = "Landscape Settings|NavBound")
    TSubclassOf<ANavMeshBoundsVolume> NavBoundClass;

    UPROPERTY(EditAnywhere, Category = "Landscape Settings|NavBound")
    float ZSize{ 100.f };

    UPROPERTY(EditAnywhere, Category = "Landscape Settings|NavBound")
    TEnumAsByte<ECollisionChannel> TerrainChannel{ ECC_WorldStatic };  // 지형 콜리전 채널

    UPROPERTY(EditAnywhere, Category = "Landscape Settings|NavBound")
    float TraceStartZ{ 200000.f };   // 위에서

    UPROPERTY(EditAnywhere, Category = "Landscape Settings|NavBound")
    float TraceEndZ{ -200000.f };    // 아래까지
private:
    TMap<FIntPoint, TUniquePtr<FSpawnerQuadNode>> SectionTrees{};

    FVector LastReconcilePos{ FVector(-1e18) };   // 첫 폴에서 무조건 1회 reconcile


    TArray<const FPalSpawnHabitatRow*> CachedRows;
    bool bRowsCached = false;

    FTimerHandle PollTimerHandle;
    float        LeafExtent = 0.f;

    void EnsureRowsCached();
    void StartPollTimer();
    void OnPollTimer();                                                 // 타이머: 리프 변경 시에만 reconcile

    void BuildCandidatesForSection(const FGenerateSectionData& SectionData);

    void ReconcileNode(FSpawnerQuadNode* Node, const FVector2D& P);      // 쿼드트리 range query (가지치기)
    bool IsNodeWithinRadius(const FSpawnerQuadNode* Node, const FVector2D& P) const;

    void ActivateNode(FSpawnerQuadNode* Node);
    void DeactivateNode(FSpawnerQuadNode* Node);

    APalMonsterSpawner* AcquireSpawner(UDataTable* Dt, const FVector& Loc);
    void ReleaseSpawner(APalMonsterSpawner* S);

    FVector GetPlayerLocation() const;
};