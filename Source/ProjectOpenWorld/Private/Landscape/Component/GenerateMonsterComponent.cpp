#include "Landscape/Component/GenerateMonsterComponent.h"
#include "Pal/Actor/PalMonsterSpawner.h"
#include "Landscape/Component/GeneratorSectionComponent.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "Pal/DataTable/PalSpawnerPlacementDatabaseRow.h"
#include "Pal/Character/PalBaseMonster.h"
//
//UGenerateMonsterComponent::UGenerateMonsterComponent()
//{
//    PrimaryComponentTick.bCanEverTick = false;
//}
//
//void UGenerateMonsterComponent::BeginPlay()
//{
//    Super::BeginPlay();
//    EnsureSectionIndexBuilt();
//}
//
//void UGenerateMonsterComponent::StartGenerateWorld(bool bEditor)
//{
//    Super::StartGenerateWorld(bEditor);
//    // 에디터 프리뷰 경로(PostEditChangeProperty)에선 BeginPlay가 안 불리므로 여기서도 보장
//    EnsureSectionIndexBuilt();
//}
//
//void UGenerateMonsterComponent::NewGenerateWorld(const FGenerateSectionData& SectionData)
//{
//    if (!SectionData.Vertices || SectionData.Vertices->Num() <= 1)
//        return;
//
//    const TArray<FPalSpawnerPlacementDatabaseRow>* Rows = SectionPlacementIndex.Find(SectionData.SectionID);
//    if (!Rows || Rows->IsEmpty())
//        return;
//
//    UWorld* World = GetWorld();
//    if (!World) return;
//
//    TArray<TObjectPtr<ACharacter>>& Spawned = SpawnedMonsterMap.FindOrAdd(SectionData.SectionID);
//    if (!Spawned.IsEmpty()) return;          // 동일 섹션 중복 스폰 방지
//    Spawned.Reserve(Rows->Num());
//
//    for (const FPalSpawnerPlacementDatabaseRow& Row : *Rows)
//    {
//        UClass* Cls = Row.SpawnerClass.Get();
//        if (!Cls || !Cls->IsChildOf(APalBaseMonster::StaticClass()))
//            continue;
//
//        FActorSpawnParameters Params;
//        Params.Owner = GetOwner();
//        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
//#if WITH_EDITOR
//        if (!World->HasBegunPlay())
//            Params.ObjectFlags |= RF_Transient;   // 에디터 프리뷰에서 맵에 저장되지 않도록
//#endif
//
//        APalBaseMonster* Character = World->SpawnActor<APalBaseMonster>(Cls, Row.Location, FRotator::ZeroRotator, Params);
//        if (Character)
//            Spawned.Add(Character);
//    }
//}
//
//void UGenerateMonsterComponent::DelGenerateWorld(const FGenerateSectionData& SectionData)
//{
//    TArray<TObjectPtr<ACharacter>>* Spawned = SpawnedMonsterMap.Find(SectionData.SectionID);
//    if (!Spawned) return;
//
//    for (TObjectPtr<ACharacter>& Character : *Spawned)
//    {
//        if (IsValid(Character))
//            Character->Destroy();
//    }
//    SpawnedMonsterMap.Remove(SectionData.SectionID);
//}
//
//void UGenerateMonsterComponent::EnsureSectionIndexBuilt()
//{
//    if (bIndexBuilt) 
//        return;
//    if (!SpawnerPlacementDataTable || !GeneratorSectionComponent)
//        return;
//
//    TArray<FPalSpawnerPlacementDatabaseRow*> Rows;
//    SpawnerPlacementDataTable->GetAllRows<FPalSpawnerPlacementDatabaseRow>(
//        TEXT("UGenerateSpawnerComponent::EnsureSectionIndexBuilt"), Rows);
//
//    SectionPlacementIndex.Reserve(Rows.Num());
//    for (const FPalSpawnerPlacementDatabaseRow* Row : Rows)
//    {
//        if (!Row) continue;
//        const FIntPoint SectionID = GeneratorSectionComponent->GetSectionIndex(Row->Location);
//        SectionPlacementIndex.FindOrAdd(SectionID).Add(*Row);  // 값 복사 저장
//    }
//    bIndexBuilt = true;
//}
//

//
//UGenerateMonsterComponent::UGenerateMonsterComponent()
//{
//	PrimaryComponentTick.bCanEverTick = false;
//}
//
//void UGenerateMonsterComponent::BeginPlay()
//{
//	Super::BeginPlay();
//}
//
//void UGenerateMonsterComponent::EnsureSectionIndexBuilt()
//{
//}
//
//void UGenerateMonsterComponent::StartGenerateWorld(bool bEditor)
//{
//}
//
//void UGenerateMonsterComponent::NewGenerateWorld(const FGenerateSectionData& SectionData)
//{
//	if (SectionData.Vertices->Num() <= 1 && !GetWorld()->HasBegunPlay())
//		return;
//	FVector2D SectionID = SectionData.SectionID;
//	TArray<TObjectPtr<APalMonsterSpawner>>& SpawnerSet =	SpawnedMonsterSpawnerSet.FindOrAdd(SectionID);
//	if (!SpawnerSet.IsEmpty())
//		return;
//	int SectionIndex = static_cast<int>(FMath::PerlinNoise2D(SectionID * 0.1) * 100000);
//	float RandomSeed = SpawnerSeed * SectionIndex;
//	if (bRandomSeed)
//	{
//		FMath::RandInit(FDateTime::Now().GetMillisecond());
//		RandomSeed = FMath::Rand();
//	}
//	FMath::RandInit(RandomSeed);
//	int Number = FMath::Rand() % SpawnerCount + 1;
//	SpawnerSet.Reserve(Number);
//	FVector StartPos = *SectionData.Vertices->begin();
//	FVector EndPos = SectionData.Vertices->Last();
//	for (int i = 0; i < Number; i++)
//	{
//		FVector SpawnPos = FVector(FMath::RandRange(StartPos.X, EndPos.X), FMath::RandRange(StartPos.Y, EndPos.Y), 0);
//		FHitResult HitResult;
//		if (GetWorld()->LineTraceSingleByChannel(
//			OUT HitResult,
//			FVector(SpawnPos.X, SpawnPos.Y, SpawnPos.Z - 12000.0f),
//			FVector(SpawnPos.X, SpawnPos.Y, SpawnPos.Z + 12000.0f),
//			ECollisionChannel::ECC_Visibility,
//			FCollisionQueryParams::DefaultQueryParam
//		))
//		{
//			SpawnPos.Z += HitResult.ImpactPoint.Z;
//		}
//		if (SpawnPos.Z >= 0)
//		{
//			APalMonsterSpawner* Spawner = Cast< APalMonsterSpawner>(GetWorld()->SpawnActor(APalMonsterSpawner::StaticClass(), &SpawnPos));
//			if (Spawner)
//			{
//				Spawner->SetSpawnList(RandomSeed);
//				SpawnerSet.Add(Spawner);
//			}
//		}
//	}
//}
//
//
//void UGenerateMonsterComponent::DelGenerateWorld(const FGenerateSectionData& SectionData)
//{
//	if(GetWorld()->HasBegunPlay())
//		return;
//	FVector2D SectionID = SectionData.SectionID;
//	if (TArray<TObjectPtr<APalMonsterSpawner>>* SpawnerSet = SpawnedMonsterSpawnerSet.Find(SectionID))
//	{
//		for(TObjectPtr<APalMonsterSpawner> Spawner : *SpawnerSet)
//		{
//			Spawner->Destroy();
//		}
//		SpawnerSet->Empty();
//	}
//	SpawnedMonsterSpawnerSet.Remove(SectionID);
//}
//
//
