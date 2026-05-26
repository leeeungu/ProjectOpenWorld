#pragma once

#include "CoreMinimal.h"
#include "Landscape/Component/GenerateWorldComponent.h"
#include "Pal/DataTable/PalSpawnHabitatRow.h"
#include "GenerateSpawnerComponent.generated.h"

class APalMonsterSpawner;
struct FPalSpawnHabitatRow;

/** 섹션 안의 한 스폰 후보. 활성화되면 ActiveCharacter 가 채워짐. */
USTRUCT()
struct FSpawnerCandidate
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, Category = "Spawner")
    FVector Location = FVector::ZeroVector;

    /** 활성 시 spawner 에 주입할 DT (펠 모스터 데이터). 미리 LoadSynchronous 된 hard 포인터. */
    UPROPERTY(VisibleAnywhere, Category = "Spawner")
    TObjectPtr<UDataTable> SpawnerDt = nullptr;

    UPROPERTY(VisibleAnywhere, Category = "Spawner")
    TObjectPtr<APalMonsterSpawner> ActiveSpawner = nullptr;
};

USTRUCT()
struct FSpawnerSectionData
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, Category = "Spawner")
    TArray<FSpawnerCandidate> Candidates;

    UPROPERTY(VisibleAnywhere, Category = "Spawner")
    bool bActive = false;
};

/** TMap UPROPERTY 중첩 회피용 풀 래퍼. */
USTRUCT()
struct FSpawnerActorPool
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<TObjectPtr<ACharacter>> Items;
};

/**
 * 룰 기반 스포너:
 *  - NewGenerateWorld    : 들어온 섹션의 정점/노멀을 sampling 해 habitat 룰과 매칭, 후보만 적재
 *  - FinishGenerateWorld : 플레이어 주변 (2R+1)^2 섹션 ring 을 다시 계산해 활성/비활성 전환
 *  - DelGenerateWorld    : 6x6 에서 빠진 섹션 정리
 *  - 활성 시 SpawnActor / 풀에서 reuse, 비활성 시 풀에 반납 또는 Destroy
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTOPENWORLD_API UGenerateSpawnerComponent : public UGenerateWorldComponent
{
    GENERATED_BODY()
    UPROPERTY()
    TArray<TObjectPtr<APalMonsterSpawner>> ActorPool{};

    UPROPERTY(EditAnywhere, Category = "Spawner", meta = (ClampMin = 0, EditCondition = "bUseActorPool"))
    int32 PoolCap = 128;
public:
    UGenerateSpawnerComponent();

    virtual void StartGenerateWorld(bool bEditor = false) override;
    virtual void NewGenerateWorld(const FGenerateSectionData& SectionData) override;
    virtual void DelGenerateWorld(const FGenerateSectionData& SectionData) override;
    virtual void FinishGenerateWorld() override;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, Category = "Spawner")
    TObjectPtr<UDataTable> HabitatDataTable = nullptr;

    /** 플레이어 주변 활성 반경. 1 = 3x3 = 9 섹션, 2 = 5x5 = 25. GeneratorSectionComponent.SectionRadiusCount / 2 미만으로 둘 것. */
    UPROPERTY(EditAnywhere, Category = "Spawner", meta = (ClampMin = 0, ClampMax = 2))
    int32 ActiveSectionRadius = 1;

    /** ON: 디스폰 시 hidden 처리 후 풀에 보관, OFF: Destroy. */
    UPROPERTY(EditAnywhere, Category = "Spawner")
    bool bUseActorPool = true;

    /** 풀 사이즈 캡 (클래스당). */
    UPROPERTY(EditAnywhere, Category = "Spawner", meta = (ClampMin = 0, EditCondition = "bUseActorPool"))
    int32 PoolCapPerClass = 64;

    UPROPERTY(EditAnywhere, Category = "Spawner|Debug")
    bool bDebugDraw = false;

    /** 섹션별 후보 + 활성 상태. (6x6 영역 안의 섹션만 들어 있음) */
    UPROPERTY(VisibleAnywhere, Category = "Spawner")
    TMap<FIntPoint, FSpawnerSectionData> SectionSpawnerMap;

private:
    /** 현재 ring (디스폰/스폰 diff 계산용) */
    TSet<FIntPoint> ActiveSectionSet;

    /** habitat row 캐시 (DT 1회 순회) */
    TArray<const FPalSpawnHabitatRow*> CachedRows;
    bool bRowsCached = false;

    void EnsureRowsCached();
    void BuildCandidatesForSection(const FGenerateSectionData& SectionData);
    void UpdateActiveRing();
    void ActivateSection(const FIntPoint& ID);
    void DeactivateSection(const FIntPoint& ID);
    APalMonsterSpawner* AcquireSpawner(UDataTable* Dt, const FVector& Loc);
    void ReleaseSpawner(APalMonsterSpawner* C);
};