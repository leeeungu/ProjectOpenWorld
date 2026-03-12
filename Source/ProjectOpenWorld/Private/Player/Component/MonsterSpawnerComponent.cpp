#include "Player/Component/MonsterSpawnerComponent.h"
#include "NavigationSystem.h"
#include "NavigationInvokerComponent.h"
#include "Creature/Character/BaseMonster.h"
#include "Pal/Subsystem/PalCharacterDataSubsystem.h"
#include "Pal/DataTable/PalMonsterData.h"

UMonsterSpawnerComponent::UMonsterSpawnerComponent() : USphereComponent()
{
	SphereRadius = 20000.f;
	//Script/Engine.DataTable'/Game/Pal/DataTable/DT_PalMonsterData.DT_PalMonsterData'
	ConstructorHelpers::FObjectFinder<UDataTable> DTMonster(TEXT("/Game/Pal/DataTable/DT_PalMonsterData.DT_PalMonsterData"));
	if (DTMonster.Succeeded())
	{
		MonsterDataTable = DTMonster.Object;
	}
	SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void UMonsterSpawnerComponent::BeginPlay()
{
	Super::BeginPlay();
	OnComponentEndOverlap.AddUniqueDynamic(this, &UMonsterSpawnerComponent::EndSpawnerOverlap);
	while (SpawnedMonsters.Num() < MaxSpawnedMonsters)
	{
		SpawnMonster();
	}
}

FPalMonsterLevelData UMonsterSpawnerComponent::GetMonsterLevelData(const FPalMonsterData* SpawnData, int32 Level) const
{
	if (SpawnData && SpawnData->LevelDataTable)
	{
		TArray<FPalMonsterLevelData*> LevelDataRows{};
		SpawnData->LevelDataTable->GetAllRows("", LevelDataRows);
		if (LevelDataRows.IsValidIndex(Level))
		{
			return *LevelDataRows[Level];
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("Monster Level Data not found : %s Level : %d"), *SpawnData->MonsterName.ToString(), Level);
	}
	else if (SpawnData && !SpawnData->LevelDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("Monster Data Table is nullptr : %s"), *SpawnData->MonsterName.ToString());
	}
	return FPalMonsterLevelData{};
}

void UMonsterSpawnerComponent::SpawnMonster()
{
	if (SpawnedMonsters.Num() < MaxSpawnedMonsters)
	{
		TArray<FPalMonsterData*> SpawnDataList{};
		MonsterDataTable->GetAllRows<FPalMonsterData>(TEXT(""), SpawnDataList);
		const FPalMonsterData* SpawnData = SpawnDataList[FMath::RandRange(0, SpawnDataList.Num() - 1)];

		UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
		FNavLocation SpawnLocation{};
		while (SpawnLocation.Location.Z <= 0)
		{
			NavSys->GetRandomReachablePointInRadius(GetComponentLocation(), SphereRadius, SpawnLocation);
		}
		FRotator SpawnRotation = GetComponentRotation();
		SpawnLocation.Location.Z += 300.0f;
		SpawnRotation.Yaw += FMath::RandRange(-180.f, 180.f);
		FActorSpawnParameters SpawnParams{};
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		ABaseMonster* SpawnedMonster = GetWorld()->SpawnActor<ABaseMonster>(SpawnData->MonsterClass, SpawnLocation.Location, SpawnRotation, SpawnParams);
		if (SpawnedMonster)
		{
			//SpawnedMonster->OnDestroyed.AddUniqueDynamic(this, &UMonsterSpawnerComponent::OnMonsterDead);
			SpawnedMonsters.Add(SpawnedMonster);
			int lv = 1;
			const FSpawnCharacterData* SpawnerData{};
			if (UPalCharacterDataSubsystem::GetPalSpawnCharacterData(SpawnData->MonsterName, SpawnerData))
			{
				lv = FMath::RandRange(SpawnerData->LvMin, SpawnerData->LvMax);
			}
			FPalMonsterLevelData LevelData = GetMonsterLevelData(SpawnData, lv);
			SpawnedMonster->SetPalMonsterLevelData(lv, LevelData);
		}
	}
	else
	{
	/*	if (!GetWorld()->GetTimerManager().IsTimerPaused(SpawnTimerHandle))
		{
			GetWorld()->GetTimerManager().PauseTimer(SpawnTimerHandle);
		}*/
	}
}

void UMonsterSpawnerComponent::OnMonsterDead(AActor* DestroyedActor)
{
	//if (DestroyedActor && SpawnedMonsters.Contains(DestroyedActor))
	//{
	//	SpawnedMonsters.Remove(DestroyedActor);

	//	//GetWorld()->GetTimerManager().UnPauseTimer(SpawnTimerHandle);	
	//}

}

void UMonsterSpawnerComponent::EndSpawnerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && SpawnedMonsters.Contains(OtherActor))
	{
		if (!IAttackInterface::Execute_IsDead(OtherActor))
		{
			OtherActor->Destroy();
		}
		SpawnedMonsters.Remove(OtherActor);
	}
	if (SpawnedMonsters.Num() < MaxSpawnedMonsters)
	{
		GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &UMonsterSpawnerComponent::SpawnMonster, SpawnTime, false);
	}
}

