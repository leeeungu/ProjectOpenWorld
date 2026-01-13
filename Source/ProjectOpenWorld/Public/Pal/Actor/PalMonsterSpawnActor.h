#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pal/DataTable/PalMonsterData.h"
#include "PalMonsterSpawnActor.generated.h"

struct FPalWildSpawnerDatabaseRow;
struct FPalSpawnerPlacementDatabaseRow;
class ABaseMonster;
class UNavigationInvokerComponent;

UCLASS()
class PROJECTOPENWORLD_API APalMonsterSpawnActor : public AActor
{
	GENERATED_BODY()
	struct PalMonsterData
	{
		//TArray<const FPalMonsterLevelData*> LevelData{};
		const FPalMonsterData* MonsterData = nullptr;
		TArray<TObjectPtr<ABaseMonster>> SpawnedMonsters{};
	};

	struct PalSpawnerData
	{
		const FPalSpawnerPlacementDatabaseRow* SpawnerData = nullptr;
		const FPalWildSpawnerDatabaseRow* WildSpawnerData = nullptr;
	};
protected:
	TMap<FName, PalMonsterData> PalMonsterDataMap{};
	TMap<FName, PalSpawnerData> SpawnerDataMap{};

	FPalMonsterLevelData DefaultMonsterLevelData{};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Spawner")
	TObjectPtr<UDataTable> WildSpawnerDataTable = nullptr;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Spawner")
	TObjectPtr<UDataTable> SpawnerPlacementDataTable = nullptr;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Spawner")
	TObjectPtr<UDataTable> MonsterDataTable = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Navigation")
	TObjectPtr<UNavigationInvokerComponent> NavigationInvokerComp = nullptr;
public:	
	APalMonsterSpawnActor();

protected:
	virtual void BeginPlay() override;

	const FPalMonsterLevelData* GetMonsterLevelData(FName MonsterName, int32 Level) const;
public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Spawner")
	void SpawnMonster(FName SpanwerName);

};
