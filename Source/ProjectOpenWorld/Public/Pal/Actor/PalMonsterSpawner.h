#pragma once

#include "CoreMinimal.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "Pal/DataTable/PalMonsterData.h"
#include "PalMonsterSpawner.generated.h"

class APalBaseMonster;
class UNavigationInvokerComponent;

UCLASS()
class PROJECTOPENWORLD_API APalMonsterSpawner : public AActor
{
    GENERATED_BODY()

public:
    APalMonsterSpawner();

    /**
     * 한 펠 종의 DT (FPalMonsterData 1 row) 를 받아 스폰 루프 시작.
     * 풀에서 reuse 될 때마다 호출 — 첫 줄에서 Teardown 하므로 안전.
     */
    UFUNCTION(BlueprintCallable, Category = "Spawner")
    void Initialize(UDataTable* InMonsterDt);

    /** 자식 몬스터 destroy + 타이머 정리. 풀 반납/Destroy 직전 호출. */
    UFUNCTION(BlueprintCallable, Category = "Spawner")
    void Teardown();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(VisibleAnywhere, Category = "Spawner")
    TObjectPtr<UNavigationInvokerComponent> NavigationInvokerComp = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
    float StaticRadius = 3000.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
    float SpawnRadius = 1000.f;
    /** Initialize 로 주입된 단일 펠 DT (1 row 가정) */
    UPROPERTY(VisibleAnywhere, Category = "Spawner")
    TObjectPtr<UDataTable> MonsterDt = nullptr;

    /** DT 의 1 row 캐시 (Initialize 시점 복사) */
    UPROPERTY(VisibleAnywhere, Category = "Spawner")
    FPalMonsterData MonsterData{};

    /** 이번 사이클 목표 몬스터 수 (NumMin~NumMax 추첨) */
    UPROPERTY(VisibleAnywhere, Category = "Spawner")
    int32 TargetNum = 0;

    UPROPERTY(VisibleAnywhere, Category = "Spawner")
    TSet<TObjectPtr<APalBaseMonster>> AliveMonsters{};

    FTimerHandle SpawnTimerHandle{};

    void StartSpawnLoop();
    void StopSpawnLoop();

    UFUNCTION()
    void OnSpawnTick();

    UFUNCTION()
    void OnMonsterDead(AActor* DeadPal);

    FPalMonsterLevelData GetLevelData(int32 Level) const;
};