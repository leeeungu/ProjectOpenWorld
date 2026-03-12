#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pal/DataTable/PalMonsterData.h"
#include "GameBase/Interface/GenerateWorldInterface.h"
#include "PalMonsterSpawnActor.generated.h"

struct FPalWildSpawnerDatabaseRow;
struct FPalSpawnerPlacementDatabaseRow;
class ABaseMonster;
class UNavigationInvokerComponent;

UCLASS()
class PROJECTOPENWORLD_API APalMonsterSpawnActor : public AActor, public IGenerateWorldInterface
{
	GENERATED_BODY()
	struct PalMonsterData
	{
		//TArray<const FPalMonsterLevelData*> LevelData{};
		const FPalMonsterData* MonsterData = nullptr;
		int CurrentCount{};
	};

	struct PalSpawnerData
	{
		FName SpawnerName{};
		const FPalSpawnerPlacementDatabaseRow* SpawnerData = nullptr;
		const FPalWildSpawnerDatabaseRow* WildSpawnerData = nullptr;
		float CurrentTIme{};
	};
protected:
	TMap<FName, PalMonsterData> PalMonsterDataMap{};
	TMap<FName, PalSpawnerData> SpawnerDataMap{};
	TMap<TObjectPtr<ABaseMonster>, FName> SpawnedMonsters{};
	TSet<PalSpawnerData*> SpawnSet{};

	FPalMonsterLevelData DefaultMonsterLevelData{};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Spawner")
	TObjectPtr<UDataTable> WildSpawnerDataTable = nullptr;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Spawner")
	TObjectPtr<UDataTable> SpawnerPlacementDataTable = nullptr;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Spawner")
	TSoftObjectPtr<UDataTable> MonsterDataTable = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Navigation")
	TObjectPtr<UNavigationInvokerComponent> NavigationInvokerComp = nullptr;
public:	
	APalMonsterSpawnActor();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnMonsterPalDestoryed(AActor* DeadPal);
	const FPalMonsterLevelData* GetMonsterLevelData(FName MonsterName, int32 Level) const;
public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Spawner")
	void SpawnMonster(FName SpanwerName);

	// IGenerateWorldInterface implementation
	virtual void  NewGenerateWorldEvent(const FGenerateSectionData& SectionData) override;
	virtual void  DelGenerateWorldEvent(const FGenerateSectionData& SectionData) override;


};
