#include "Building/Subsystem/BuildingDataSubsystem.h"
#include "Building/Actor/BuildingActor.h"

UBuildingDataSubsystem* UBuildingDataSubsystem::SingletonInstance{};

void UBuildingDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	SingletonInstance = this;
	//Script/Engine.DataTable'/Game/Building/DataTable/DT_BuildObjectDataTable.DT_BuildObjectDataTable'
	if (!LoadAndSaveDataTableToMap(PalBuildObjectDataTable, TEXT("/Game/Building/DataTable/DT_BuildObjectDataTable.DT_BuildObjectDataTable")))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load PalBuildObjectData DataTable"));
	}
	//Script/Engine.DataTable'/Game/Building/DataTable/DT_BuildObjectIconDataTable.DT_BuildObjectIconDataTable'
	if (!LoadAndSaveDataTableToMap(PalBuildObjectIconDataTable, TEXT("/Game/Building/DataTable/DT_BuildObjectIconDataTable.DT_BuildObjectIconDataTable")))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load PalBuildObjectIconData DataTable"));
	}
}

bool UBuildingDataSubsystem::GetPalBuildObjectData(FName RowName, const FPalBuildObjectData*& Data)
{
	if (!SingletonInstance)
		return false;
	if (const FPalBuildObjectData* const* FoundData = SingletonInstance->PalBuildObjectDataTable.ItemDataMap.Find(RowName))
	{
		Data = *FoundData;
		return true;
	}
	Data = &SingletonInstance->PalBuildObjectDataTable.Dummy;
	return false;
}

bool UBuildingDataSubsystem::GetPalBuildObjectIconData(FName RowName, const FPalBuildObjectIconData*& Data)
{
	if (!SingletonInstance)
		return false;
	if (const FPalBuildObjectIconData* const* FoundData = SingletonInstance->PalBuildObjectIconDataTable.ItemDataMap.Find(RowName))
	{
		Data = *FoundData;
		return true;
	}
	Data = &SingletonInstance->PalBuildObjectIconDataTable.Dummy;
	return false;
}

TSubclassOf<ABuildingActor> UBuildingDataSubsystem::GetPalBuildObjectBuildingClassByName(FName RowName)
{
	const FPalBuildObjectIconData* Result{};
	GetPalBuildObjectIconData(RowName, Result);
	if (Result)
		return Result->BuildingClass;
	return TSubclassOf<ABuildingActor>();
}

UStaticMesh* UBuildingDataSubsystem::GetPalBuildObjectBuildingMeshByName(FName RowName)
{
	/*const FPalBuildObjectIconData* Result{};
	GetPalBuildObjectIconData(RowName, Result);
	if (Result)
		return Result->BuildingMesh;*/
	return nullptr;
}

UTexture2D* UBuildingDataSubsystem::GetPalBuildObjectIconTextureByName(FName RowName)
{
	const FPalBuildObjectIconData* Result{};
	GetPalBuildObjectIconData(RowName, Result);
	if (Result)
		return Result->SoftIcon;
	return nullptr;
}

FString UBuildingDataSubsystem::GetPalBuildObjectMapObjectIdByName(FName RowName)
{
	const FPalBuildObjectData* Result{};
	GetPalBuildObjectData(RowName, Result);
	if (Result)
		return Result->MapObjectId;
	return FString();
}

int32 UBuildingDataSubsystem::GetPalBuildObjectSortIdByName(FName RowName)
{
	const FPalBuildObjectData* Result{};
	GetPalBuildObjectData(RowName, Result);
	if (Result)
		return Result->SortId;
	return int32();
}

int32 UBuildingDataSubsystem::GetPalBuildObjectBuildCapacityByName(FName RowName)
{
	const FPalBuildObjectData* Result{};
	GetPalBuildObjectData(RowName, Result);
	if (Result)
		return Result->BuildCapacity;
	return int32();
}

float UBuildingDataSubsystem::GetPalBuildObjectRequiredBuildWorkAmountByName(FName RowName)
{
	const FPalBuildObjectData* Result{};
	GetPalBuildObjectData(RowName, Result);
	if (Result)
		return Result->RequiredBuildWorkAmount;
	return 0.0f;
}

int32 UBuildingDataSubsystem::GetPalBuildObjectAssetValueByName(FName RowName)
{
	const FPalBuildObjectData* Result{};
	GetPalBuildObjectData(RowName, Result);
	if (Result)
		return Result->AssetValue;
	return int32();
}

TArray<FBuildMaterialData> UBuildingDataSubsystem::GetPalBuildObjectMaterialsByName(FName RowName)
{
	const FPalBuildObjectData* Result{};
	GetPalBuildObjectData(RowName, Result);
	if (Result)
		return Result->Materials;
	return TArray<FBuildMaterialData>();
}

float UBuildingDataSubsystem::GetPalBuildObjectInstallNeighborThresholdByName(FName RowName)
{
	const FPalBuildObjectData* Result{};
	GetPalBuildObjectData(RowName, Result);
	if (Result)
		return Result->InstallNeighborThreshold;
	return 0.0f;
}

bool UBuildingDataSubsystem::GetPalBuildObjectbIsInstallOnlyOnBaseByName(FName RowName)
{
	const FPalBuildObjectData* Result{};
	GetPalBuildObjectData(RowName, Result);
	if (Result)
		return Result->bIsInstallOnlyOnBase;
	return false;
}

bool UBuildingDataSubsystem::GetPalBuildObjectbIsInstallOnlyInDoorByName(FName RowName)
{
	const FPalBuildObjectData* Result{};
	GetPalBuildObjectData(RowName, Result);
	if (Result)
		return Result->bIsInstallOnlyInDoor;
	return false;
}

bool UBuildingDataSubsystem::GetPalBuildObjectbIsInstallOnlyHubAroundByName(FName RowName)
{
	const FPalBuildObjectData* Result{};
	GetPalBuildObjectData(RowName, Result);
	if (Result)
		return Result->bIsInstallOnlyHubAround;
	return false;
}

int32 UBuildingDataSubsystem::GetPalBuildObjectInstallMaxNumInBaseCampByName(FName RowName)
{
	const FPalBuildObjectData* Result{};
	GetPalBuildObjectData(RowName, Result);
	if (Result)
		return Result->bIsInstallOnlyOnBase;
	return int32();
}

bool UBuildingDataSubsystem::GetPalBuildObjectbInstallableNoObstacleFromCameraByName(FName RowName)
{
	const FPalBuildObjectData* Result{};
	GetPalBuildObjectData(RowName, Result);
	if (Result)
		return Result->bInstallableNoObstacleFromCamera;
	return false;
}
