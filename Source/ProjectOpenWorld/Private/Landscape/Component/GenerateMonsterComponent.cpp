#include "Landscape/Component/GenerateMonsterComponent.h"
#include "Pal/Actor/PalMonsterSpawner.h"

UGenerateMonsterComponent::UGenerateMonsterComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGenerateMonsterComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UGenerateMonsterComponent::NewGenerateWorld(const FGenerateSectionData& SectionData)
{
	if (SectionData.Vertices->Num() <= 1)
		return;
	FVector2D SectionID = SectionData.SectionID;
	TArray<TObjectPtr<APalMonsterSpawner>>& SpawnerSet =	SpawnedMonsterSpawnerSet.FindOrAdd(SectionID);
	if (!SpawnerSet.IsEmpty())
		return;
	int SectionIndex = static_cast<int>(FMath::PerlinNoise2D(SectionID * 0.1) * 100000);
	float RandomSeed = SpawnerSeed * SectionIndex;
	if (bRandomSeed)
	{
		FMath::RandInit(FDateTime::Now().GetMillisecond());
		RandomSeed = FMath::Rand();
	}
	FMath::RandInit(RandomSeed);
	int Number = FMath::Rand() % SpawnerCount + 1;
	SpawnerSet.Reserve(Number);
	FVector StartPos = *SectionData.Vertices->begin();
	FVector EndPos = SectionData.Vertices->Last();
	for (int i = 0; i < Number; i++)
	{
		FVector SpawnPos = FVector(FMath::RandRange(StartPos.X, EndPos.X), FMath::RandRange(StartPos.Y, EndPos.Y), 0);
		FHitResult HitResult;
		if (GetWorld()->LineTraceSingleByChannel(
			OUT HitResult,
			FVector(SpawnPos.X, SpawnPos.Y, SpawnPos.Z - 12000.0f),
			FVector(SpawnPos.X, SpawnPos.Y, SpawnPos.Z + 12000.0f),
			ECollisionChannel::ECC_Visibility,
			FCollisionQueryParams::DefaultQueryParam
		))
		{
			SpawnPos.Z += HitResult.ImpactPoint.Z;
		}
		APalMonsterSpawner* Spawner = Cast< APalMonsterSpawner>(GetWorld()->SpawnActor(APalMonsterSpawner::StaticClass(), &SpawnPos));
		if (Spawner)
		{
			Spawner->SetSpawnList(RandomSeed);
			SpawnerSet.Add(Spawner);
		}
	}
}


void UGenerateMonsterComponent::DelGenerateWorld(const FGenerateSectionData& SectionData)
{
	FVector2D SectionID = SectionData.SectionID;
	if (TArray<TObjectPtr<APalMonsterSpawner>>* SpawnerSet = SpawnedMonsterSpawnerSet.Find(SectionID))
	{
		for(TObjectPtr<APalMonsterSpawner> Spawner : *SpawnerSet)
		{
			Spawner->Destroy();
		}
		SpawnerSet->Empty();
	}
	SpawnedMonsterSpawnerSet.Remove(SectionID);
}


