#include "Pal/Actor/PalMonsterSpawner.h"
#include "NavigationSystem.h"
#include "NavigationInvokerComponent.h"
#include "Creature/Character/BaseMonster.h"
#include "Pal/Subsystem/PalCharacterDataSubsystem.h"

APalMonsterSpawner::APalMonsterSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	NavigationInvokerComp = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("NavigationInvokerComp"));
	///Script/Engine.DataTable'/Game/Pal/DataTable/DT_PalMonsterData.DT_PalMonsterData'
	ConstructorHelpers::FObjectFinder<UDataTable> DTMonster(TEXT("/Game/Pal/DataTable/DT_PalMonsterData.DT_PalMonsterData"));
	if (DTMonster.Succeeded())
	{
		MonsterDataTable = DTMonster.Object;
	}
}

void APalMonsterSpawner::BeginPlay()
{
	Super::BeginPlay();
}

void APalMonsterSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	for (auto& SpawnSet : SpawnedMonsters)
	{
		for (AActor* Monster : SpawnSet.Value)
		{
			Monster->Destroy();
		}
		SpawnSet.Value.Reset();
	}
	SpawnedMonsters.Reset();
	for (auto& SpawnMap : PalMonsterDataMap)
	{
		FSpawnListData* SpawnData = &SpawnMap.Value;
		if (GetWorld()->GetTimerManager().IsTimerActive(SpawnData->SpawnTimerHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(SpawnData->SpawnTimerHandle);
		}
	}
}

void APalMonsterSpawner::SetSpawnTimer(FName MonsterName)
{
	FSpawnListData* SpawnData = PalMonsterDataMap.Find(MonsterName);

	if (SpawnData)
	{
		const FSpawnCharacterData* SpawnerData{};
		if (UPalCharacterDataSubsystem::GetPalSpawnCharacterData(SpawnData->MonsterData.MonsterName, SpawnerData))
		{
			GetWorld()->GetTimerManager().SetTimer(
				SpawnData->SpawnTimerHandle,
				[this, MonsterName]()
				{
					OnSpawnMonster(MonsterName);
				},
				SpawnerData->SpawnTime, true, SpawnerData->SpawnTime
			);
		}
	}
}

void APalMonsterSpawner::OnSpawnMonster(FName MonsterName)
{
	FMath::RandInit(SpawnerSeed);
	const FSpawnListData* SpawnData = PalMonsterDataMap.Find(MonsterName);
	TSet<TObjectPtr<AActor>>& SpawnSet = SpawnedMonsters.FindOrAdd(MonsterName);
	if (SpawnData && SpawnSet.Num() < SpawnData->Num)
	{
		UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
		FNavLocation SpawnLocation{};
		while (SpawnLocation.Location.Z <= 0)
		{
			NavSys->GetRandomReachablePointInRadius(GetActorLocation(), StaticRadius, SpawnLocation);
		}
		FRotator SpawnRotation = GetActorRotation();
		SpawnLocation.Location.Z += 300.0f;
		FActorSpawnParameters SpawnParams{};
		ABaseMonster* SpawnedMonster = GetWorld()->SpawnActor<ABaseMonster>(SpawnData->MonsterData.MonsterClass, SpawnLocation.Location, SpawnRotation, SpawnParams);
		if (SpawnedMonster)
		{
			SpawnedMonster->OnDestroyed.AddUniqueDynamic(this, &APalMonsterSpawner::OnMonsterDead);
			SpawnSet.Add(SpawnedMonster);
			int lv = 1;
			const FSpawnCharacterData* SpawnerData{};
			if (UPalCharacterDataSubsystem::GetPalSpawnCharacterData(SpawnData->MonsterData.MonsterName, SpawnerData))
			{
				lv = FMath::RandRange(SpawnerData->LvMin, SpawnerData->LvMax);
			}
			FPalMonsterLevelData LevelData = GetMonsterLevelData(SpawnData->MonsterData.MonsterName, lv);
			SpawnedMonster->SetPalMonsterLevelData(lv, LevelData);
		}
	}
	else
	{
		if (SpawnData && !GetWorld()->GetTimerManager().IsTimerPaused(SpawnData->SpawnTimerHandle))
		{
			GetWorld()->GetTimerManager().PauseTimer(SpawnData->SpawnTimerHandle);
		}
	}
}

FPalMonsterLevelData APalMonsterSpawner::GetMonsterLevelData(FName MonsterName, int32 Level) const
{
	FSpawnListData const* MonsterData = PalMonsterDataMap.Find(MonsterName);
	if (MonsterData  && MonsterData->MonsterData.LevelDataTable)
	{
		TArray<FPalMonsterLevelData*> LevelDataRows{};
		MonsterData->MonsterData.LevelDataTable->GetAllRows("", LevelDataRows);
		if (LevelDataRows.IsValidIndex(Level))
		{
			return *LevelDataRows[Level];
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("Monster Level Data not found : %s Level : %d"), *MonsterData->MonsterData.MonsterName.ToString(), Level);
	}
	else if (!MonsterData || !MonsterData->MonsterData.LevelDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("Monster Data Table is nullptr : %s"), *MonsterName.ToString());
	}
	return FPalMonsterLevelData{};
}

void APalMonsterSpawner::OnMonsterDead(AActor* DeadPal)
{
	ABaseMonster* Monster = Cast<ABaseMonster>(DeadPal);
	if (Monster)
	{
		FName MonsterName = Monster->GetMonsterName();
		TSet<TObjectPtr<AActor>>* SpawnSet = SpawnedMonsters.Find(MonsterName);
		if (SpawnSet)
		{
			SpawnSet->Remove(Monster);
		}
		FSpawnListData* SpawnData = PalMonsterDataMap.Find(MonsterName);
		if (SpawnData && GetWorld()->GetTimerManager().IsTimerPaused(SpawnData->SpawnTimerHandle))
		{
			GetWorld()->GetTimerManager().UnPauseTimer(SpawnData->SpawnTimerHandle);
		}
	}
}

void APalMonsterSpawner::SetSpawnList(float RandSeed)
{
	SpawnerSeed = RandSeed;
	if (MonsterDataTable)
	{
		TArray<FPalMonsterData*> MonsterDataRows{};
		MonsterDataTable->GetAllRows<FPalMonsterData>(TEXT(""), MonsterDataRows);
		for (const FPalMonsterData* MonsterDataRow : MonsterDataRows)
		{
			if (MonsterDataRow)
			{
				FSpawnListData& List = PalMonsterDataMap.Add(MonsterDataRow->MonsterName, {});
				List.MonsterData = *MonsterDataRow;
			}
		}
	}
	for (auto& SpawnMap : PalMonsterDataMap)
	{
		FSpawnListData* SpawnData = &SpawnMap.Value;
		FMath::RandInit(SpawnerSeed);
		const FSpawnCharacterData* SpawnerData{};
		if (UPalCharacterDataSubsystem::GetPalSpawnCharacterData(SpawnData->MonsterData.MonsterName, SpawnerData))
		{
			SpawnData->Num = FMath::RandRange(SpawnerData->NumMin, SpawnerData->NumMax);
			SetSpawnTimer(SpawnMap.Key);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Spawn Character Data not found : %s"), *SpawnData->MonsterData.MonsterName.ToString());
		}
	}
}

//void APalMonsterSpawner::NewGenerateWorldEvent(const FGenerateSectionData& SectionData)
//{
//	bIsSpawning = true;
//}
//
//void APalMonsterSpawner::DelGenerateWorldEvent(const FGenerateSectionData& SectionData)
//{
//	bIsSpawning = false;
//	for (auto& SpawnSet : SpawnedMonsters)
//	{
//		for (AActor* Monster : SpawnSet.Value)
//		{
//			Monster->Destroy();
//		}
//		SpawnSet.Value.Reset();
//	}
//	SpawnedMonsters.Reset();
//}

//void APalMonsterSpawner::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//}

