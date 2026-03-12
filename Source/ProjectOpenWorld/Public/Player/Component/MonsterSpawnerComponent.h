#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "MonsterSpawnerComponent.generated.h"

struct FPalMonsterData;
struct FPalMonsterLevelData;

UCLASS()
class PROJECTOPENWORLD_API UMonsterSpawnerComponent : public USphereComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MonsterSpawner")
	TObjectPtr<UDataTable> MonsterDataTable = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MonsterSpawner")
	TSet<TObjectPtr< AActor> > SpawnedMonsters{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MonsterSpawner", meta = (ClampMin = 1))
	int32 MaxSpawnedMonsters = 50;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MonsterSpawner", meta = (ClampMin = 1))
	float SpawnTime = 5.0f;


	FTimerHandle SpawnTimerHandle{};
	bool CanSpawnable = true;
public:
	UMonsterSpawnerComponent();

	void SetSpawnable(bool bSpawnable);
protected:
	virtual void BeginPlay() override;


	FPalMonsterLevelData GetMonsterLevelData(const FPalMonsterData* SpawnData, int32 Level) const;

	UFUNCTION()
	void SpawnMonster();

	UFUNCTION()
	void OnMonsterDead(AActor* DestroyedActor);
	UFUNCTION()
	void EndSpawnerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
