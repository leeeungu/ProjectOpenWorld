#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Building/DataTable/PalBuildObjectData.h"
#include "BuildingDataSubsystem.generated.h"

class UTexture2D;
class UStaticMesh;
class ABuildingActor;

UCLASS()
class PROJECTOPENWORLD_API UBuildingDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
private:
	template<typename T>
	struct DataTableStruct
	{
		UPROPERTY()
		TObjectPtr<UDataTable> ItemDataTable{};
		UPROPERTY()
		TMap<FName, const T*> ItemDataMap{}; // Key: Editor_RowNameHash
		T Dummy{};
	};
	static UBuildingDataSubsystem* SingletonInstance;
protected:

	DataTableStruct< FPalBuildObjectData> PalBuildObjectDataTable{};
	DataTableStruct< FPalBuildObjectIconData> PalBuildObjectIconDataTable{};

	template<typename T>
	bool LoadAndSaveDataTableToMap(DataTableStruct<T>& DataStruct, FString Path)
	{
		TObjectPtr<UDataTable>& DataTable = DataStruct.ItemDataTable;
		DataTable = LoadObject<UDataTable>(nullptr, *Path);
		if (!DataTable)
		{
			return false;
		}
		TArray<FName> RowNames = DataTable->GetRowNames();
		for (const FName& RowName : RowNames)
		{
			T* Data = DataTable->FindRow<T>(RowName, "");
			if (Data)
			{
				DataStruct.ItemDataMap.Add(RowName, Data);
			}
		}
		return true;
	}

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	static bool IsValidInstance() { return SingletonInstance != nullptr; }
	static bool GetPalBuildObjectData(FName RowName, const FPalBuildObjectData*& Data);
	static bool GetPalBuildObjectIconData(FName RowName, const FPalBuildObjectIconData*& Data);

	//FString MapObjectId{};
	//int32 SortId = 0;
	//int32 BuildCapacity = 0;
	//float RequiredBuildWorkAmount = 0.0f;
	//int32 AssetValue = 0;
	//TArray<FBuildMaterialData> Materials{};
	//float InstallNeighborThreshold = 0.0f;
	//bool bIsInstallOnlyOnBase = false;
	//bool bIsInstallOnlyInDoor = false;
	//bool bIsInstallOnlyHubAround = false;
	//int32 InstallMaxNumInBaseCamp = 0;
	//bool bInstallableNoObstacleFromCamera = false;
	UFUNCTION(BlueprintPure, Category = "BuildingDataSubsystem")
	static TSubclassOf< ABuildingActor> GetPalBuildObjectBuildingClassByName(FName RowName);
	UFUNCTION(BlueprintPure, Category = "BuildingDataSubsystem")
	static UStaticMesh* GetPalBuildObjectBuildingMeshByName(FName RowName);

	UFUNCTION(BlueprintPure, Category = "BuildingDataSubsystem")
	static UTexture2D* GetPalBuildObjectIconTextureByName(FName RowName);
	UFUNCTION(BlueprintPure, Category = "BuildingDataSubsystem")
	static FString GetPalBuildObjectMapObjectIdByName(FName RowName);
	UFUNCTION(BlueprintPure, Category = "BuildingDataSubsystem")
	static int32 GetPalBuildObjectSortIdByName(FName RowName);
	UFUNCTION(BlueprintPure, Category = "BuildingDataSubsystem")
	static int32 GetPalBuildObjectBuildCapacityByName(FName RowName);
	UFUNCTION(BlueprintPure, Category = "BuildingDataSubsystem")
	static float GetPalBuildObjectRequiredBuildWorkAmountByName(FName RowName);
	UFUNCTION(BlueprintPure, Category = "BuildingDataSubsystem")
	static int32 GetPalBuildObjectAssetValueByName(FName RowName);
	UFUNCTION(BlueprintPure, Category = "BuildingDataSubsystem")
	static TArray<FBuildMaterialData> GetPalBuildObjectMaterialsByName(FName RowName);
	UFUNCTION(BlueprintPure, Category = "BuildingDataSubsystem")
	static float GetPalBuildObjectInstallNeighborThresholdByName(FName RowName);
	UFUNCTION(BlueprintPure, Category = "BuildingDataSubsystem")
	static bool GetPalBuildObjectbIsInstallOnlyOnBaseByName(FName RowName);
	UFUNCTION(BlueprintPure, Category = "BuildingDataSubsystem")
	static bool GetPalBuildObjectbIsInstallOnlyInDoorByName(FName RowName);
	UFUNCTION(BlueprintPure, Category = "BuildingDataSubsystem")
	static bool GetPalBuildObjectbIsInstallOnlyHubAroundByName(FName RowName);
	UFUNCTION(BlueprintPure, Category = "BuildingDataSubsystem")
	static int32 GetPalBuildObjectInstallMaxNumInBaseCampByName(FName RowName);
	UFUNCTION(BlueprintPure, Category = "BuildingDataSubsystem")
	static bool GetPalBuildObjectbInstallableNoObstacleFromCameraByName(FName RowName);








	//static UTexture2D* GetPalItemIconTextureByName(FName RowName);
};
