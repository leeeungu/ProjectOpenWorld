#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Pal/DataTable/PalWildSpawnerDatabaseRow.h"
#include "Pal/Data/StatusData.h"
#include "PalMonsterData.generated.h"

enum class EStatusType : uint8;
class APalBaseMonster;
class APalBaseCreature;

USTRUCT(BlueprintType)
struct FPalMonsterPatrolData 
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Patrol Data")
	TArray<FVector> PatrolPoint{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Patrol Data")
	bool bLoop = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Patrol Data")
	bool bRandom = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Patrol Data")
	float RandomRadius = 100.0f;
};

USTRUCT(BlueprintType)
struct FPalMonsterLevelData : public FTableRowBase // Row Equl to Level
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Level Data")
	TMap<EStatusType, double> LevelStatusData{};
};

// Monster World Data
USTRUCT(BlueprintType)
struct FPalMonsterData : public FTableRowBase 
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Data")
	FName MonsterName = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Data")
	TSubclassOf< APalBaseMonster> MonsterClass = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PalWildSpawnerDatabaseRow")
	TSubclassOf<APalBaseCreature> CreatureClass = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Level Data")
	FPalMonsterPatrolData PatrolData{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Level Data")
	TObjectPtr<UDataTable> LevelDataTable{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster Spawn Data", meta = (ClampMin = 1))
	int32 LvMin = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster Spawn Data", meta = (ClampMax = 255))
	int32 LvMax = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster Spawn Data", meta = (ClampMin = 0))
	int32 NumMin = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster Spawn Data", meta = (ClampMax = 10))
	int32 NumMax = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster Spawn Data", meta = (ClampMin = 1))
	float SpawnTime = 1.0f;
};