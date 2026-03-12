#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pal/DataTable/PalMonsterData.h"
#include "GameBase/Interface/GenerateWorldInterface.h"
#include "PalMonsterSpawner.generated.h"

class ABaseMonster;
class UNavigationInvokerComponent;

USTRUCT(BlueprintType)
struct FSpawnListData
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Spawner")
	FPalMonsterData MonsterData{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Spawner")
	int Num = 0;

	FTimerHandle SpawnTimerHandle{};
};

class UDataTable;
UCLASS()
class PROJECTOPENWORLD_API APalMonsterSpawner : public AActor
{
	GENERATED_BODY()
protected:
	bool bIsSpawning{ false };

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Spawner")
	TObjectPtr<UNavigationInvokerComponent> NavigationInvokerComp = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Spawner")
	TSoftObjectPtr<UDataTable> MonsterDataTable = nullptr;
	//UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Spawner")
	TMap<FName, FSpawnListData> PalMonsterDataMap{};
	TMap<FName, TSet<TObjectPtr<AActor>>> SpawnedMonsters{};


	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Spawner")
	float StaticRadius = 1000.0f;

	float SpawnerSeed = 0.0f;
public:	
	APalMonsterSpawner();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	FPalMonsterLevelData GetMonsterLevelData(FName MonsterName, int32 Level) const;

	void SetSpawnTimer(FName MonsterName);
	UFUNCTION()
	void OnSpawnMonster(FName MonsterName);
	UFUNCTION()
	void OnMonsterDead(AActor* DeadPal);
public:	
	void SetSpawnList(float RandSeed);

};
