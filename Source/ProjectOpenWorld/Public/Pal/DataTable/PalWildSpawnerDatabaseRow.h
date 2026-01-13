#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PalWildSpawnerDatabaseRow.generated.h"
//"Pal_1" : "SheepBall",
//        "NPC_1" : "None",
//        "LvMin_1" : 1,
//        "LvMax_1" : 2,
//        "NumMin_1" : 2,
//        "NumMax_1" : 3,
//        "Pal_2" : "PinkCat",
//        "NPC_2" : "None",
//        "LvMin_2" : 1,
//        "LvMax_2" : 2,
//        "NumMin_2" : 3,
//        "NumMax_2" : 4,
//        "Pal_3" : "None",
//        "NPC_3" : "None",
//        "LvMin_3" : 0,
//        "LvMax_3" : 0,
//        "NumMin_3" : 0,
//        "NumMax_3" : 0,


USTRUCT(BlueprintType)
struct FSpawnCharacterData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PalWildSpawnerDatabaseRow")
	FName Pal = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PalWildSpawnerDatabaseRow")
	FName NPC = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PalWildSpawnerDatabaseRow", meta = (ClampMin = 1))
	int32 LvMin = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PalWildSpawnerDatabaseRow", meta = (ClampMax = 255))
	int32 LvMax = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PalWildSpawnerDatabaseRow", meta = (ClampMin = 0))
	int32 NumMin = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PalWildSpawnerDatabaseRow", meta = (ClampMax = 10))
	int32 NumMax = 1;
};
//{
//    "SpawnerName": "Test",
//        "SpawnerType" : "EPalSpawnedCharacterType::Common",
//        "Weight" : 10.0,
//        "OnlyTime" : "EPalOneDayTimeType::Undefined",
//        "OnlyWeather" : "EPalWeatherConditionType::Undefined",
//      
//        "bIsAllowRandomizer" : true
//      },
USTRUCT(BlueprintType)
struct FPalWildSpawnerDatabaseRow : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PalWildSpawnerDatabaseRow")
	FName SpawnerName = NAME_None;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PalWildSpawnerDatabaseRow")
	//FName SpawnerType;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PalWildSpawnerDatabaseRow")
	//float Weight;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PalWildSpawnerDatabaseRow")
	//FName OnlyTime{};
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PalWildSpawnerDatabaseRow")
	//FName OnlyWeather{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PalWildSpawnerDatabaseRow")
	TArray<FSpawnCharacterData> Pal{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PalWildSpawnerDatabaseRow")
	bool bIsAllowRandomizer{};

};