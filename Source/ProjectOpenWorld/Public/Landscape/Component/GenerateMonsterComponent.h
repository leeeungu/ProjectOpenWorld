#pragma once

#include "CoreMinimal.h"
#include "Landscape/Component/GenerateWorldComponent.h"
#include "GenerateMonsterComponent.generated.h"

class APalMonsterSpawner;

/*
 세션 별로 몬스터를 생성하도록 하려 했지만 너무 먼 세션에도 과도하게 character가 생성되어 렉 발생
현재 맵의 크기를 생각하면 너무 과도하게 생성되어 폐기
=> 생각의 전환으로 Player가 스포너를 가지고 있게 하면 자동으로 근처에만 생성될 것이고 
   player와 어느정도 멀어지면 몬스터를 제거하도록 하면 충분히 최적화 될 것으로 생각됨

   => UMonsterSpawnerComponent로 기능 이전 (고정 스폰은 힘들수도..)
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UGenerateMonsterComponent : public UGenerateWorldComponent
{
	GENERATED_BODY()
protected:
	//UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Landscape Settings")
	TMap<FVector2D, TArray<TObjectPtr<APalMonsterSpawner>>> SpawnedMonsterSpawnerSet{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings")
	int SpawnerCount = 5;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings")
	float SpawnerSeed = 100;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings")
	bool bRandomSeed{};
public:	
	UGenerateMonsterComponent();

protected:
	virtual void BeginPlay() override;

	virtual void StartGenerateWorld(bool bEditor = false) override;
	virtual void NewGenerateWorld(const FGenerateSectionData& SectionData) override;
	virtual void DelGenerateWorld(const FGenerateSectionData& SectionData) override;

};
