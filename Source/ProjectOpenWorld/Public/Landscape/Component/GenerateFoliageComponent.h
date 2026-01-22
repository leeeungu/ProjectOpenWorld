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
		TMap<TObjectPtr< UFoliageInstancedStaticMeshComponent>, TSet<int32>> NewInstanceIndex{}; // FoliageID, Instance Indices
		bool bSectionNew{};
		bool bAllReadyNew = false;
	};

	struct FoliageUpdateData
	{
		TObjectPtr<UFoliageInstancedStaticMeshComponent> FoliageComponent{};
		FTransform NewTransform{};
		FIntPoint SectionID{};
		int32 InstanceIndex{};
		int32 FoliageID{};
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
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings")
	TArray<FFoliageDataTable> TestFoliageTypes{};
	TArray<FFoliageDataTable*> FoliageTypes{};
	//UPROPERTY(VisibleAnywhere, Category = "Landscape Settings")
	UPROPERTY()
	TMap< TObjectPtr<UFoliageType_InstancedStaticMesh>, TObjectPtr< UFoliageInstancedStaticMeshComponent>> FoliageTypeToInstanceIndex{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings")
	int32 FoliageMaxCount = 10000;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings", meta = (ClampMin = "1", ClampMax = "200"))
	float FoliageComponentCount = 10;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings", meta = (ClampMin = "1", ClampMax = "100"))
	int32 UpdateTickCount = 10;	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings")
	FIntPoint SectionRange{ 20,40};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings")
	float FoliageSeed = 100;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings")
	bool bRandomSeed{};
	TMap<TObjectPtr< UFoliageInstancedStaticMeshComponent>, TSet<int32>> RemoveInstanceIndex{}; // FoliageID, Instance Indices
	TArray< FoliageUpdateData> UpdateData{};
	TArray< FoliageUpdateData> UpdateBackData{};
	TMap <FIntPoint, FSectionData> SectionIDToFoliageTypeToInstanceIndex{};
	TArray<FIntPoint> DeleteArray{};
	TArray<FoliageAddData> AddArray{};

	int nSectionIndex{};
	//int CurrentUpdateTick = 0;
	bool bFoliageUpdated = false;
	bool EditorModeGenerate = false;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Landscape Settings")
	int nInstanceCount{};
	bool bGeneratingFoliage = true;
	bool UpdateBuffer{};
public:	
	UGenerateFoliageComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void StartGenerateWorld(bool bEditor = false) override;
	virtual void NewGenerateWorld(const FGenerateSectionData& SectionData) override;
	virtual void DelGenerateWorld(const FGenerateSectionData& SectionData) override;
	//virtual void NewGenerateWorld(FIntPoint SectionID, const TArray<FVector>& Vertices, const TArray<FVector2D>& UVs, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FProcMeshTangent>& Tangents) override;
	//virtual void DelGenerateWorld(FIntPoint SectionID, const TArray<FVector>& Vertices, const TArray<FVector2D>& UVs, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FProcMeshTangent>& Tangents) override;
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