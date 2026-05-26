#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PalSpawnHabitatRow.generated.h"

class AActor;
class UDataTable;

USTRUCT(BlueprintType)
struct FPalSpawnHabitatRow : public FTableRowBase
{
    GENERATED_BODY()
public:
    FPalSpawnHabitatRow() = default;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Habitat")
    TSoftObjectPtr<UDataTable> SpawnerDt{};                  // = DT_{Pal}_MonsterData 키

    /** Perlin Z 하한(cm) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Habitat")
    float MinHeight = 0.f;

    /** Perlin Z 상한(cm) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Habitat")
    float MaxHeight = 5000.f;

    /** 최대 경사도(도). normal.Z 로 변환해서 비교 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Habitat", meta = (ClampMin = 0, ClampMax = 90))
    float MaxSlope = 30.f;

    /** 섹션당 평균 스폰 후보 수. 소수 가능 (0.05 = 20섹션당 1개) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Habitat", meta = (ClampMin = 0))
    float SectionDensity = 3.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Habitat", meta = (ClampMin = 0))
    float StaticRadius = 15000.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Habitat", meta = (ClampMin = 0))
    float RespawnCoolTime = 60.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Habitat", meta = (ClampMin = 0))
    int32 SpawnCount = 3;
};