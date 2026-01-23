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
		FoliageSectionData(int32 SectionID = -1, float Random = 0.0f)
		{
			SectionIndex = SectionID;
			RandomSeed = Random;
		}
		
		bool bAlreadyGenerate{ false };

	private:
		int32 SectionIndex{};
		float RandomSeed{ };

		TMap<TObjectPtr<UFoliageType_InstancedStaticMesh>, int32> StaticMeshMap{};
		TArray<TObjectPtr< UFoliageInstancedStaticMeshComponent>> MeshComponent{};
	public:
		void Clear_Async()
		{
			bAlreadyGenerate = false;
			StaticMeshMap.Empty();
			MeshComponent.Empty();
		}
		bool GetAlreadyGenerate() const
		{
			return bAlreadyGenerate;
		}

		bool ContainsFoliageMesh(TObjectPtr<UFoliageType_InstancedStaticMesh> FoliageMesh)
		{
			return StaticMeshMap.Contains(FoliageMesh);
		}

		void SetFoliageMeshData(TObjectPtr<UFoliageType_InstancedStaticMesh> FoliageMesh, TObjectPtr< UFoliageInstancedStaticMeshComponent> MeshComp)
		{
			bAlreadyGenerate = true;
			if (!StaticMeshMap.Contains(FoliageMesh))
			{
				int32 NewIndex = MeshComponent.Add(MeshComp);
				NewIndex = MeshComponent.Num() - 1;
				StaticMeshMap.Add(FoliageMesh, NewIndex);
			}
		}
		bool GetFoliageMeshData(TObjectPtr<UFoliageType_InstancedStaticMesh> FoliageMesh, TObjectPtr< UFoliageInstancedStaticMeshComponent>& OutMeshComp) const
		{
			if (const int32* FoundIndex = StaticMeshMap.Find(FoliageMesh))
			{
				OutMeshComp = MeshComponent[*FoundIndex];
				return true;
			}
			return false;
		}
		TArray<TObjectPtr< UFoliageInstancedStaticMeshComponent>> GetAllFoliageMeshComponent_Async() const
		{
			return MeshComponent;
		}

		TArray<TObjectPtr< UFoliageType_InstancedStaticMesh>> GetAllFoliageMeshType_Async() const
		{
			TArray<TObjectPtr< UFoliageType_InstancedStaticMesh>> FoliageMeshArray{};
			StaticMeshMap.GetKeys(FoliageMeshArray);
			return FoliageMeshArray;
		}

		float GetRandomSeed_Async() const
		{
			return RandomSeed;
		}
	};
	
	struct FoliageUpdateData
	{
		FIntPoint SectionID{};
		TObjectPtr<UFoliageType_InstancedStaticMesh> FoliageMesh{};
		TObjectPtr<UFoliageInstancedStaticMeshComponent> MeshComponent{};
		TArray<FTransform>  FoliageData{};
		bool bAdd{};
		void Rset()
		{
			SectionID = FIntPoint{ INT32_MIN, INT32_MIN };
			FoliageMesh = nullptr;
			MeshComponent = nullptr;
			FoliageData.Empty();
			bAdd = false;
		}
		FoliageUpdateData& operator=(const FoliageUpdateData& Other)
		{
			if (this != &Other)
			{
				SectionID = Other.SectionID;
				FoliageMesh = Other.FoliageMesh;
				MeshComponent = Other.MeshComponent;
				FoliageData = Other.FoliageData;
				bAdd = Other.bAdd;
			}
			return *this;
		}
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

	TQueue<TObjectPtr<UFoliageInstancedStaticMeshComponent>> EmpthyComponentArray{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings", meta = (ClampMin = "1", ClampMax = "1000"))
	int32 FoliageComponentCount = 800;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Landscape Settings")
	int32 EmptyComponentCount{};

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

	TMap<FIntPoint, TMap<TObjectPtr<UFoliageInstancedStaticMeshComponent>, FoliageUpdateData>> UpdateBackData{};
	TArray<FoliageUpdateData> UpdateData{};

	TMap <FIntPoint, FoliageSectionData> SectionIDToFoliageTypeToInstanceIndex{};
	TSet<FIntPoint> DeleteArray{};
	TMap<FIntPoint, FoliageAddData> AddMap{};

	int32 nSectionIndex{};
	int32 nUpdateTickIndex{};
	bool EditorModeGenerate{};
	bool bUpdateBackData{};
	bool bGeneratingFoliage{};
	bool bDelayUpdate{ false };
	FIntPoint PlayerLastSectionID{ INT32_MIN, INT32_MIN };
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