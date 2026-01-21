// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PalSpawnerPlacementDatabaseRow.generated.h"


//"InstanceName": "BP_PalSpawner_Sheets_sanctuary_11_1_1_grass_C_UAID_FC349764B52D91A901_1131289641",
//"SpawnerName" : "sanctuary_11_1_1_grass",
//"SpawnerType" : "EPalSpawnedCharacterType::Common",
//"PlacementType" : "EPalSpawnerPlacementType::Field",
//"Location" : {
//    "X": -456082.66,
//        "Y" : 185386.48,
//        "Z" : 1934.1459
//},
//"RadiusType" : "EPalSpawnRadiusType::S",
//"StaticRadius" : 15000.0,
//"WorldName" : "PL_MainWorld5",
//"LayerNames" : [
//    "Spawners"
//] ,
//"SpawnerClass" : {
//    "AssetPathName": "/Game/Pal/Blueprint/Spawner/SheetsVariant/BP_PalSpawner_Sheets_sanctuary_11_1_1_grass.BP_PalSpawner_Sheets_sanctuary_11_1_1_grass_C",
//        "SubPathString" : ""
//},
//"RespawnCoolTime" : 0.0

USTRUCT(BlueprintType)
struct FPalSpawnerPlacementDatabaseRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	FPalSpawnerPlacementDatabaseRow() = default;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnerPlacement")
	FName InstanceName = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnerPlacement")
	FName SpawnerName = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnerPlacement")
	FVector Location = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnerPlacement")
	double StaticRadius = 0.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnerPlacement")
	TSubclassOf<UObject> SpawnerClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnerPlacement")
	float RespawnCoolTime = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnerPlacement")
	uint8 SpawnCount{};

};

