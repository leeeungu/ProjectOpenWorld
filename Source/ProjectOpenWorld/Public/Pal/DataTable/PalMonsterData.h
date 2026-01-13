#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PalMonsterData.generated.h"


USTRUCT(BlueprintType)
struct FPalMonsterPatrolData 
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Patrol Data")
	TArray<FVector> PatrolPoint{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Patrol Data")
	bool bLoop = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Patrol Data")
	bool bRandom = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Patrol Data")
	float RandomRadius = 100.0f;
};

USTRUCT(BlueprintType)
struct FPalMonsterLevelData : public FTableRowBase // Row Equl to Level
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Level Data")
	int32 MaxHP = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Level Data")
	float AttackPower = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Level Data")
	float Armor = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Level Data")
	float MoveSpeed = 500.0f;
};


class ABaseMonster;

USTRUCT(BlueprintType)
struct FPalMonsterData : public FTableRowBase 
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Data")
	FName MonsterName = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Data")
	TSubclassOf< ABaseMonster> MonsterClass = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Patrol Data")
	TObjectPtr<UWorld> PatrolLevel = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Level Data")
	FPalMonsterPatrolData PatrolData{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Level Data")
	TObjectPtr<UDataTable> LevelDataTable{};
};