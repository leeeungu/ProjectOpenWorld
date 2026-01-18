#pragma once

#include "CoreMinimal.h"
#include "Landscape/Component/GenerateWorldComponent.h"
#include "GenerateFoliageComponent.generated.h"

class UInstancedStaticMeshComponent;
class UFoliageType_InstancedStaticMesh;
class UFoliageInstancedStaticMeshComponent;
class ULandscapeGrassType;
class UGrassInstancedStaticMeshComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UGenerateFoliageComponent : public UGenerateWorldComponent
{
	struct FFoliageData
	{
		FTransform InstanceTransform{};
		int32 FoliageID{};
		FIntPoint SectionID{};
	};
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings")
	TArray<TObjectPtr<UFoliageType_InstancedStaticMesh>> FoliageTypes{};
	UPROPERTY(VisibleAnywhere, Category = "Landscape Settings")
	TArray<TObjectPtr< UFoliageInstancedStaticMeshComponent>> FoliageInstances{};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings")
	int32 FoliageMaxCount = 1000;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings")
	int32 FoliageType = 100;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings")
	int32 UpdateTickCount = 1;

	TArray< FFoliageData>  arInstancesTranform{};

	TArray<FIntPoint> DeleteArray{};
	TMap <FIntPoint, int32> SectionIDToFoliageTypeToInstanceIndex{};
	// Key = SectionID, Value : nSectionIndex 
	TArray<TMap<int32, TSet<int32>>>  arrFoliageSectionIndex{};
	// FoliageType, Key = nSectionIndex Value : index

	int nSectionIndex{};
	int CurrentUpdateTick = 0;
	bool bFoliageUpdated = false;
public:	
	UGenerateFoliageComponent();

protected:
	virtual void BeginPlay() override;

	void UpdateFoliageInstances();
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void StartGenerateWorld() override;
	virtual void NewGenerateWorld(FIntPoint SectionID, const TArray<FVector>& Vertices, const TArray<FVector2D>& UVs, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FProcMeshTangent>& Tangents) override;
	virtual void DelGenerateWorld(FIntPoint SectionID, const TArray<FVector>& Vertices, const TArray<FVector2D>& UVs, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FProcMeshTangent>& Tangents) override;
	virtual void FinishGenerateWorld() override;

public:
	virtual void Initialize(USceneComponent* ParentComponent) override;
};
