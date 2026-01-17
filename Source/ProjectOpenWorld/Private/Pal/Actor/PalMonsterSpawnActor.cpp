#include "Pal/Actor/PalMonsterSpawnActor.h"
#include "Pal/DataTable/PalSpawnerPlacementDatabaseRow.h"
#include "Pal/DataTable/PalWildSpawnerDatabaseRow.h"
#include "Kismet/DataTableFunctionLibrary.h"
#include "Pal/Character/BasePalMonster.h"
#include "NavigationSystem.h"
#include "NavigationInvokerComponent.h"
#include "Pal/Component/PalPatrolComponent.h"


APalMonsterSpawnActor::APalMonsterSpawnActor()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	NavigationInvokerComp = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("NavigationInvokerComp"));
}

void APalMonsterSpawnActor::BeginPlay()
{
	Super::BeginPlay();
	if (WildSpawnerDataTable && SpawnerPlacementDataTable)
	{
		TArray<FPalWildSpawnerDatabaseRow*> WildSpawnerDataRows{};
		WildSpawnerDataTable->GetAllRows<FPalWildSpawnerDatabaseRow>(TEXT(""), WildSpawnerDataRows);

		TArray<FPalSpawnerPlacementDatabaseRow*> SpawnerPlacementDataRows{};
		SpawnerPlacementDataTable->GetAllRows<FPalSpawnerPlacementDatabaseRow>(TEXT(""), SpawnerPlacementDataRows);
		for (const FPalWildSpawnerDatabaseRow* WildSpawnerDataRow : WildSpawnerDataRows)
		{
			PalSpawnerData& SpawnerData = SpawnerDataMap.FindOrAdd(WildSpawnerDataRow->SpawnerName, PalSpawnerData{});
			SpawnerData.WildSpawnerData = WildSpawnerDataRow;
		}

		for (const FPalSpawnerPlacementDatabaseRow* SpawnerPlacementDataRow : SpawnerPlacementDataRows)
		{
			PalSpawnerData& SpawnerData = SpawnerDataMap.FindOrAdd(SpawnerPlacementDataRow->SpawnerName, PalSpawnerData{});
			SpawnerData.SpawnerData = SpawnerPlacementDataRow;
		}
	}

	if (MonsterDataTable)
	{
		TArray<FPalMonsterData*> MonsterDataRows{};
		MonsterDataTable->GetAllRows<FPalMonsterData>(TEXT(""), MonsterDataRows);
		for (const FPalMonsterData* MonsterDataRow : MonsterDataRows)
		{
			if (MonsterDataRow)
			{
				PalMonsterData& MonsterData = PalMonsterDataMap.FindOrAdd(MonsterDataRow->MonsterName, PalMonsterData{});
				MonsterData.MonsterData = MonsterDataRow;
			}
		}
	}
}

const FPalMonsterLevelData* APalMonsterSpawnActor::GetMonsterLevelData(FName MonsterName, int32 Level) const
{
	PalMonsterData const* MonsterData = PalMonsterDataMap.Find(MonsterName);
	if (MonsterData && MonsterData->MonsterData && MonsterData->MonsterData->LevelDataTable)
	{
		TArray<FPalMonsterLevelData*> LevelDataRows{};
		MonsterData->MonsterData->LevelDataTable->GetAllRows("", LevelDataRows);
		if (LevelDataRows.IsValidIndex(Level))
		{
			return LevelDataRows[Level];
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("Monster Level Data not found : %s Level : %d"), *MonsterData->MonsterData->MonsterName.ToString(), Level);
	}
	else if (!MonsterData || !MonsterData->MonsterData || !MonsterData->MonsterData->LevelDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("Monster Data Table is nullptr : %s"), *MonsterName.ToString());
	}
	return &DefaultMonsterLevelData;
}

void APalMonsterSpawnActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APalMonsterSpawnActor::SpawnMonster(FName SpanwerName)
{
	const PalSpawnerData* SpawnerData = SpawnerDataMap.Find(SpanwerName);
	if (SpawnerData && SpawnerData->WildSpawnerData && SpawnerData->SpawnerData)
	{
		for (const FSpawnCharacterData& SpawnCharacterData : SpawnerData->WildSpawnerData->Pal)
		{
			PalMonsterData* MonsterData = PalMonsterDataMap.Find(SpawnCharacterData.Pal);
			if (MonsterData && MonsterData->MonsterData && MonsterData->MonsterData->MonsterClass)
			{
				const int32 SpawnNum = FMath::RandRange(SpawnCharacterData.NumMin, SpawnCharacterData.NumMax);
				for (int32 i = 0; i < SpawnNum; ++i)
				{
					UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
					FNavLocation SpawnLocation{};
					NavSys->GetRandomReachablePointInRadius( SpawnerData->SpawnerData->Location, SpawnerData->SpawnerData->StaticRadius, SpawnLocation);
					FRotator SpawnRotation = GetActorRotation();
					FActorSpawnParameters SpawnParams{};
					ABaseMonster* SpawnedMonster = GetWorld()->SpawnActor<ABaseMonster>(MonsterData->MonsterData->MonsterClass, SpawnLocation.Location, SpawnRotation, SpawnParams);
					//UE_LOG(LogTemp, Warning, TEXT("Spawned Monster: %s at Location: %s"), *MonsterData->MonsterData->MonsterName.ToString(), *SpawnLocation.ToString());
					if (SpawnedMonster)
					{
						MonsterData->SpawnedMonsters.Add(SpawnedMonster);
						int lv = FMath::RandRange(SpawnCharacterData.LvMin, SpawnCharacterData.LvMax);
						const FPalMonsterLevelData* LevelData = GetMonsterLevelData(MonsterData->MonsterData->MonsterName, lv);
						SpawnedMonster->SetPalMonsterLevelData(lv, *LevelData);
					}
					ABasePalMonster* PalMonster = Cast<ABasePalMonster>(SpawnedMonster);
					if (PalMonster && PalMonster->GetPalPatrolComponent() && MonsterData->MonsterData && MonsterData->MonsterData->PatrolLevel->GetName() == GetWorld()->GetName())
					{
						UE_LOG(LogTemp, Warning, TEXT("Set Patrol Data for Monster: %s"), *MonsterData->MonsterData->MonsterName.ToString());
						PalMonster->GetPalPatrolComponent()->SetPatrolData(&MonsterData->MonsterData->PatrolData);
					}
					else if (MonsterData->MonsterData && MonsterData->MonsterData->PatrolLevel.GetName() != GetWorld()->GetName())
					{
						UE_LOG(LogTemp, Warning, TEXT("Patrol Level is different from Spawn Level for Monster: %s"), *MonsterData->MonsterData->MonsterName.ToString());
							UE_LOG(LogTemp, Warning, TEXT("Monster Patrol Level: %d, Spawn Level: %d"), MonsterData->MonsterData->PatrolLevel->GetUniqueID(), GetWorld()->GetUniqueID());
					}
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawner Data not found : %s"), *SpanwerName.ToString());
	}
}

