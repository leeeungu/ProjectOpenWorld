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
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UGenerateFoliageComponent : public UGenerateWorldComponent
{
	friend class FAsyncFoliageGenerater;

	struct FoliageSectionData
	{
		FoliageSectionData()
		{
			SectionIndex = -1;
		}
		int32 SectionIndex{};
		float RandomSeed{ };
		bool bAlreadyGenerate{ };
	};


	struct FoliageStaticMeshData
	{
		TObjectPtr<UFoliageInstancedStaticMeshComponent> FoliageMeshComponent{};
		// 백에서 array 삽입, 삭제
		TMap<FIntPoint, TArray<FTransform>> FoliageInstanceTransfomArray{}; // lay 전 data
		// 백에서 관리된 맵에서 가져옴
		TArray<FTransform> FoliageInstanceTransfomArray{};
		// 메인 tick에서 갱신
		int nTickIndex{};
	};
	
	struct FoliageUpdateData
	{
		FIntPoint SectionID{};
		TObjectPtr<UFoliageType_InstancedStaticMesh> FoliageMesh{};
		TArray<FTransform>  FoliageData{};
	};

	struct FoliageAddData
	{
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
	int32 ComponentIndex{};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings", meta = (ClampMin = "1", ClampMax = "100"))
	int32 FoliageComponentCount = 30;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings", meta = (ClampMin = "1", ClampMax = "5000"))
	int32 UpdateComponentTickCount = 10;	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings")
	FIntPoint FoliageDataCreateRange{ 20,40};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings")
	float FoliageSeed = 100;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings")
	bool bRandomSeed{};
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Landscape Settings")
	int nInstanceCount{};

	TMap<TObjectPtr<UFoliageType_InstancedStaticMesh>, FoliageStaticMeshData> FoliageComponentMap{};
	TMap <FIntPoint, FoliageSectionData> FoliageSectionDataMap{};
	TArray<FoliageUpdateData> UpdateData{};

	TSet<FIntPoint> DeleteArray{};
	TMap<FIntPoint, FoliageAddData> AddMap{};
	FIntPoint PlayerLastSectionID{ INT32_MIN, INT32_MIN };

	int32 nSectionCount{};
	int32 nUpdateTickIndex{};
	bool EditorModeGenerate{};
	bool bUpdateBackData{};
	bool bGeneratingFoliage{};
	bool bDelayUpdate{ false };
public:	
	UGenerateFoliageComponent();

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
protected:
	virtual void BeginPlay() override;
	void SetFoliageMeshComponent(TObjectPtr<UFoliageType_InstancedStaticMesh> FoliageMesh, TObjectPtr< UFoliageInstancedStaticMeshComponent> MeshComp);

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
		Initialize();
	}

	FORCEINLINE	TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FAsyncFoliageGenerater, STATGROUP_ThreadPoolAsyncTasks);
	}

	void DoWork();
private:
	void Initialize();
	UPROPERTY()
	TObjectPtr<UGenerateFoliageComponent> FoliageGenerater{};

	TSet<FIntPoint> DeleteArray{};
	TMap<FIntPoint, UGenerateFoliageComponent::FoliageAddData> AddMap{};
};