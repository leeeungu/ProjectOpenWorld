#pragma once

#include "CoreMinimal.h"
#include "Landscape/Component/GenerateWorldComponent.h"
#include "GenerateTerrainComponent.generated.h"

struct FProcMeshTangent;
class UProceduralMeshComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UGenerateTerrainComponent : public UGenerateWorldComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	TObjectPtr<UProceduralMeshComponent> TargetTerrain{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	TObjectPtr<UProceduralMeshComponent> GenerateTerrain{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	TObjectPtr< UMaterialInterface> TerrainMaterial{};
public:	
	UGenerateTerrainComponent();
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
protected:
	virtual void BeginPlay() override;

	virtual void StartGenerateWorld() override;
	virtual void UpdateGenerateWorld(int SectionIndex, const TArray<FVector>& Vertices, const TArray<FVector2D>& UVs, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FProcMeshTangent>& Tangents) override;
	//virtual void FinishGenerateWorld() override;

public:
	virtual void Initialize(USceneComponent* ParentComponent) override;

	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
