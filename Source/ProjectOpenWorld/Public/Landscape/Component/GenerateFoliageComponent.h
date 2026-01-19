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
		bool bSectionDelete = false;
	};
	struct FFoliageData
	{
		FTransform InstanceTransform{};
		int32 FoliageID{};
		FIntPoint SectionID{};
		int32 InstanceIndex{};
	};
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings")
	TObjectPtr<UDataTable> FoliageDataTable{};
	//UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings")
	TArray<FFoliageDataTable*> FoliageTypes{};
	//UPROPERTY(VisibleAnywhere, Category = "Landscape Settings")
	UPROPERTY()
	TMap< TObjectPtr<UFoliageType_InstancedStaticMesh>, TObjectPtr< UFoliageInstancedStaticMeshComponent>> FoliageTypeToInstanceIndex{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings")
	int32 FoliageMaxCount = 10000;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings", meta = (ClampMin = "1", ClampMax = "100"))
	int32 UpdateTickCount = 10;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings")
	FIntPoint SectionRange{ 20,40};
	TMap<TObjectPtr< UFoliageInstancedStaticMeshComponent>, TArray<int32>> RemoveInstanceIndex{}; // FoliageID, Instance Indices

	TArray< FFoliageData>  arInstancesTranform{};

	TMap <FIntPoint, FSectionData> SectionIDToFoliageTypeToInstanceIndex{};
	TArray<FIntPoint> DeleteArray{};

	int nSectionIndex{};
	int CurrentUpdateTick = 0;
	int InstanceIndex{};
	bool bFoliageUpdated = false;
	bool EditorModeGenerate = false;
	int nInstanceCount{};
public:	
	UGenerateFoliageComponent();

protected:
	virtual void BeginPlay() override;

	void UpdateFoliageInstances();
	void ClearFoliageInstances();
	void AddFoliageInstance(FIntPoint SectionID, FVector StartPos, FVector  EndPos);
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void StartGenerateWorld(bool bEditor = false) override;
	virtual void NewGenerateWorld(FIntPoint SectionID, const TArray<FVector>& Vertices, const TArray<FVector2D>& UVs, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FProcMeshTangent>& Tangents) override;
	virtual void DelGenerateWorld(FIntPoint SectionID, const TArray<FVector>& Vertices, const TArray<FVector2D>& UVs, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FProcMeshTangent>& Tangents) override;
	virtual void FinishGenerateWorld() override;

public:
	virtual void Initialize(USceneComponent* ParentComponent) override;
};
