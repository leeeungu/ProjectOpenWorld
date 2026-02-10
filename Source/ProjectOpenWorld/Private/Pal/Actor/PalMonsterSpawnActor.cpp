#include "Pal/Actor/PalMonsterSpawnActor.h"
#include "Pal/DataTable/PalSpawnerPlacementDatabaseRow.h"
#include "Pal/DataTable/PalWildSpawnerDatabaseRow.h"
#include "Kismet/DataTableFunctionLibrary.h"
#include "Pal/Character/BasePalMonster.h"
#include "NavigationSystem.h"
#include "NavigationInvokerComponent.h"
#include "Pal/Component/PalPatrolComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "BrainComponent.h"

APalMonsterSpawnActor::APalMonsterSpawnActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;
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
			SpawnerData.SpawnerName = WildSpawnerDataRow->SpawnerName;
			SpawnSet.Add(&SpawnerData);
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

void APalMonsterSpawnActor::OnMonsterPalDestoryed(AActor* DeadPal)
{
	if (ABaseMonster* Monster = Cast< ABaseMonster>(DeadPal))
	{
		if (FName * MonsterName = SpawnedMonsters.Find(Monster))
		{
			PalMonsterDataMap.Find(*MonsterName)->CurrentCount--;
			SpawnedMonsters.Remove(Monster);
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
	for (PalSpawnerData* Data : SpawnSet)
	{
		Data->CurrentTIme += DeltaTime;
		if (Data->CurrentTIme >= Data->SpawnerData->RespawnCoolTime)
		{
			Data->CurrentTIme -= Data->SpawnerData->RespawnCoolTime;
			SpawnMonster(Data->SpawnerName);
		}
	}
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
				if (SpawnerData->SpawnerData->SpawnCount <= MonsterData->CurrentCount)
				{
					continue;
				}
				MonsterData->CurrentCount++;
				const int32 SpawnNum = FMath::RandRange(SpawnCharacterData.NumMin, SpawnCharacterData.NumMax);
				for (int32 i = 0; i < SpawnNum; ++i)
				{
					UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
					FNavLocation SpawnLocation{};
					while (SpawnLocation.Location.Z <= 0)
					{
						NavSys->GetRandomReachablePointInRadius( GetActorLocation(), SpawnerData->SpawnerData->StaticRadius, SpawnLocation);
					}
					FRotator SpawnRotation = GetActorRotation();
					SpawnLocation.Location.Z += 300.0f;
					FActorSpawnParameters SpawnParams{};
					ABaseMonster* SpawnedMonster = GetWorld()->SpawnActor<ABaseMonster>(MonsterData->MonsterData->MonsterClass, SpawnLocation.Location, SpawnRotation, SpawnParams);
					if (SpawnedMonster)
					{
						SpawnedMonster->OnDestroyed.AddUniqueDynamic(this, &APalMonsterSpawnActor::OnMonsterPalDestoryed);
						SpawnedMonsters.Add(SpawnedMonster, SpawnCharacterData.Pal);
						int lv = FMath::RandRange(SpawnCharacterData.LvMin, SpawnCharacterData.LvMax);
						const FPalMonsterLevelData* LevelData = GetMonsterLevelData(MonsterData->MonsterData->MonsterName, lv);
						SpawnedMonster->SetPalMonsterLevelData(lv, *LevelData);
					}
					ABasePalMonster* PalMonster = Cast<ABasePalMonster>(SpawnedMonster);
					if (PalMonster && PalMonster->GetPalPatrolComponent() && MonsterData->MonsterData ) // && MonsterData->MonsterData->PatrolLevel->GetName() == GetWorld()->GetName())
					{
						//UE_LOG(LogTemp, Warning, TEXT("Set Patrol Data for Monster: %s"), *MonsterData->MonsterData->MonsterName.ToString());
						PalMonster->GetPalPatrolComponent()->SetPatrolData(&MonsterData->MonsterData->PatrolData);
					}
					/*else if (MonsterData->MonsterData && MonsterData->MonsterData->PatrolLevel.GetName() != GetWorld()->GetName()) // 레벨 교환 시 delete로 인해 crash 문제가 나서 주석
					{
						UE_LOG(LogTemp, Warning, TEXT("Patrol Level is different from Spawn Level for Monster: %s"), *MonsterData->MonsterData->MonsterName.ToString());
							UE_LOG(LogTemp, Warning, TEXT("Monster Patrol Level: %d, Spawn Level: %d"), MonsterData->MonsterData->PatrolLevel->GetUniqueID(), GetWorld()->GetUniqueID());
					}*/
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawner Data not found : %s"), *SpanwerName.ToString());
	}
}

void APalMonsterSpawnActor::NewGenerateWorldEvent(const FGenerateSectionData& SectionData)
{
	for (auto& Pal : SpawnedMonsters)
	{
		if (Pal.Key)
		{
			Pal.Key->SetActorHiddenInGame(false);
			Pal.Key->UpdateComponentVisibility();
			ACharacter* pCharacter = Cast<ACharacter>(Pal.Key);
			if (pCharacter)
			{
				pCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
				AAIController* AIController = Cast<AAIController>(pCharacter->GetController());
				if (AIController && AIController->BrainComponent)
				{
					AIController->BrainComponent->ResumeLogic(TEXT("Generate"));
				}
			}
		}
	}
}

void APalMonsterSpawnActor::DelGenerateWorldEvent(const FGenerateSectionData& SectionData)
{
	for (auto& Pal : SpawnedMonsters)
	{
		if (Pal.Key)
		{
			Pal.Key->Destroy();
			/*ABasePalMonster* pCharacter = Cast<ABasePalMonster>(Pal.Key);
			if (pCharacter)
			{
				pCharacter->StopAnimMontage();
				pCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
				AAIController* AIController = Cast<AAIController>(pCharacter->GetController());
				if (AIController && AIController->BrainComponent)
				{
					AIController->BrainComponent->PauseLogic(TEXT("Generate"));
				}
			}*/
		}
	}
	SpawnedMonsters.Empty();
}

