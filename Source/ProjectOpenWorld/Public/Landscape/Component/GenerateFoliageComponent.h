#pragma once

#include "CoreMinimal.h"
#include "Landscape/Component/GenerateWorldComponent.h"
#include "GenerateFoliageComponent.generated.h"

class UInstancedStaticMeshComponent;
class UFoliageType_InstancedStaticMesh;
class UFoliageInstancedStaticMeshComponent;
class ULandscapeGrassType;
class UGrassInstancedStaticMeshComponent;


USTRUCT(BlueprintType)
struct FFoliageInstanceData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage")
	TObjectPtr<UFoliageType_InstancedStaticMesh> FoliageType{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage")
	FVector InstanceOffset{};
};


USTRUCT(BlueprintType)
struct FFoliageDataTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage")
	TArray<FFoliageInstanceData> InstanceData{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage")
	FFloatInterval HeightRange{ 0.0f, 30000.0f };
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UGenerateFoliageComponent : public UGenerateWorldComponent
{
	friend class FAsyncFoliageGenerater;
	struct FSectionData
	{
		FSectionData()
		{
			SectionIndex = -1;
		}
		int32 SectionIndex{};
		float RandomSeed{ };
		TMap<TObjectPtr<UFoliageType_InstancedStaticMesh>, TObjectPtr< UFoliageInstancedStaticMeshComponent>> MeshComponent{}; // FoliageID, Instance Indices
		bool bAlreadyGenerate{ false };
	};

	struct FoliageUpdateData
	{
		FIntPoint SectionID{};
		int32  InstanceIndex{};
		TObjectPtr<UFoliageType_InstancedStaticMesh> FoliageMesh{};
		TObjectPtr<UFoliageInstancedStaticMeshComponent> MeshComponent{};
		TArray<FTransform>  FoliageData{};
		bool bTickGenerate{ false };
	};

	struct FoliageAddData
	{
		FIntPoint SectionID{};
		FVector StartPos{};
		FVector EndPos{};
	};

	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings")
	TObjectPtr<UDataTable> FoliageDataTable{};
	TArray<FFoliageDataTable*> FoliageTypes{};
	UPROPERTY(VisibleDefaultsOnly)
	TArray<TObjectPtr<UFoliageInstancedStaticMeshComponent>> FoliageComponentArray{};

	TArray<TObjectPtr<UFoliageInstancedStaticMeshComponent>> EmpthyComponentArray{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings", meta = (ClampMin = "1", ClampMax = "1000"))
	float FoliageComponentCount = 800;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings", meta = (ClampMin = "1", ClampMax = "1000"))
	int32 UpdateComponentTickCount = 10;	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings")
	FIntPoint FoliageDataCreateRange{ 20,40};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings")
	float FoliageSeed = 100;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings")
	bool bRandomSeed{};
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Landscape Settings")
	int nInstanceCount{};

	TArray<FoliageUpdateData> UpdateData{};
	TMap<TObjectPtr<UFoliageInstancedStaticMeshComponent>, FoliageUpdateData> UpdateBackData{};
	TMap <FIntPoint, FSectionData> SectionIDToFoliageTypeToInstanceIndex{};

	TArray<FIntPoint> DeleteArray{};
	TArray<FoliageAddData> AddArray{};

	int32 nSectionIndex{};
	int32 nUpdateTickIndex{};
	bool EditorModeGenerate{};
	bool bUpdateBackData{};
	bool bGeneratingFoliage{};
	FIntPoint PlayerLastSectionID{ INT32_MIN, INT32_MIN };
public:	
	UGenerateFoliageComponent();

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void StartGenerateWorld(bool bEditor = false) override;
	virtual void NewGenerateWorld(const FGenerateSectionData& SectionData) override;
	virtual void DelGenerateWorld(const FGenerateSectionData& SectionData) override;
	virtual void FinishGenerateWorld() override;

public:
	virtual void Initialize(USceneComponent* ParentComponent) override;
};

class FAsyncFoliageGenerater : public FNonAbandonableTask
{
public:
	FAsyncFoliageGenerater(UGenerateFoliageComponent* InFoliageGenerater)
	{
		FoliageGenerater = InFoliageGenerater;
	}

	FORCEINLINE	TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FAsyncFoliageGenerater, STATGROUP_ThreadPoolAsyncTasks);
	}

	void DoWork();
private:
	UPROPERTY()
	TObjectPtr<UGenerateFoliageComponent> FoliageGenerater{};
};