#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PalBuildObjectData.generated.h"

//"MapObjectId": "FarmBlockV2_wheet",
//"TypeA" : "EPalBuildObjectTypeA::Food",
//"SortId" : 7,
//"TypeB" : "EPalBuildObjectTypeB::Food_Basic",
//"TypeUIDisplay" : "EPalBuildObjectTypeForUIDisplay::Farm",
//"Rank" : 1,
//"BuildCapacity" : 0,
//"RequiredBuildWorkAmount" : 8000.0,
//"AssetValue" : 1,
//"RequiredEnergyType" : "EPalEnergyType::None",
//"ConsumeEnergySpeed" : 0.0,
//"Material1_Id" : "WheatSeeds",
//"Material1_Count" : 3,
//"Material2_Id" : "Wood",
//"Material2_Count" : 35,
//"Material3_Id" : "stone",
//"Material3_Count" : 35,
//"Material4_Id" : "None",
//"Material4_Count" : 0,
//"BlueprintItemID" : "None",
//"OverrideDescMsgID" : "None",
//"bInstallAtReticle" : false,
//"InstallNeighborThreshold" : 140.0,
//"bIsInstallOnlyOnBase" : false,
//"bIsInstallOnlyInDoor" : false,
//"bIsInstallOnlyHubAround" : false,
//"InstallMaxNumInBaseCamp" : 0,
//"bInstallableNoObstacleFromCamera" : false,
//"BuildExpRate" : 3.32,
//"bIsProhibitedInRaidBossArea" : false,
//"MaxBuildCountInRaidBossArea" : 0,
//"bIsPaintable" : false

USTRUCT(BlueprintType)
struct FBuildMaterialData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaterialData")
	FString Material_Id{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaterialData")
	int32 Material_Count = 0;
};


USTRUCT(BlueprintType)
struct FPalBuildObjectData : public FTableRowBase
{
	GENERATED_BODY()
public:
	static int GetMaxMaterialCount() { return 4; }
	FPalBuildObjectData();
	// Map Object Id
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalBuildObjectData")
	FString MapObjectId{};
	// Sort Id
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalBuildObjectData")
	int32 SortId = 0;
	// Build Capacity
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalBuildObjectData")
	int32 BuildCapacity = 0;
	// Required Build Work Amount
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalBuildObjectData")
	float RequiredBuildWorkAmount = 0.0f;
	// Asset Value
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalBuildObjectData")
	int32 AssetValue = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalBuildObjectData")
	TArray<FBuildMaterialData> Materials{};
	// Install Neighbor Threshold
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalBuildObjectData")
	float InstallNeighborThreshold = 0.0f;

	//"bIsInstallOnlyOnBase": false,
	//	"bIsInstallOnlyInDoor" : false,
	//	"bIsInstallOnlyHubAround" : false,
	//"InstallMaxNumInBaseCamp": 0,
	//bInstallableNoObstacleFromCamera

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalBuildObjectData")
	bool bIsInstallOnlyOnBase = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalBuildObjectData")
	bool bIsInstallOnlyInDoor = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalBuildObjectData")
	bool bIsInstallOnlyHubAround = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalBuildObjectData")
	int32 InstallMaxNumInBaseCamp = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalBuildObjectData")
	bool bInstallableNoObstacleFromCamera = false;

};
