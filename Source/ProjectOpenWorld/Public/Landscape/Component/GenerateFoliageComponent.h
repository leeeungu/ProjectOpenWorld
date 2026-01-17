#pragma once

#include "CoreMinimal.h"
#include "Landscape/Component/GenerateWorldComponent.h"
#include "GenerateFoliageComponent.generated.h"

class UInstancedStaticMeshComponent;
class UGrassInstancedStaticMeshComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UGenerateFoliageComponent : public UGenerateWorldComponent
{
	GENERATED_BODY()
protected:
	//UPROPERTY(Category = "Generate Foliage", EditInstanceOnly)
	//TArray<TObjectPtr<UInstancedStaticMeshComponent>> FoliageInstances{};


	UPROPERTY(Category = "Generate Foliage", EditAnywhere)
	TObjectPtr< UInstancedStaticMeshComponent> GrassFoliageInstance{};

	//UPROPERTY(EditAnywhere, Category = "Generate Foliage")
	//TArray< FFoliageData> FoliageData{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Generate Foliage")
	int32 FoliageType = 100;


public:	
	UGenerateFoliageComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void StartGenerateWorld() override;
	virtual void UpdateGenerateWorld(int SectionIndex, const TArray<FVector>& Vertices, const TArray<FVector2D>& UVs, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FProcMeshTangent>& Tangents) override;
	//virtual void FinishGenerateWorld() override;

public:
	virtual void Initialize(USceneComponent* ParentComponent) override;
};
