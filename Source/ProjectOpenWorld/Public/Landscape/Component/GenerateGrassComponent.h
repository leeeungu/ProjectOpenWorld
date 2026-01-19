#pragma once

#include "CoreMinimal.h"
#include "Landscape/Component/GenerateWorldComponent.h"
#include "GenerateGrassComponent.generated.h"

class ULandscapeGrassType;
class UGrassInstancedStaticMeshComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UGenerateGrassComponent : public UGenerateWorldComponent
{
	GENERATED_BODY()
protected:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings")
	TArray<TObjectPtr<ULandscapeGrassType>> GrassTypes{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings")
	TArray<TObjectPtr< UGrassInstancedStaticMeshComponent>> GrassInstances{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings")
	float GrassRadius = 1.0f;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings")
	int32 GrassMaxCount = 1000;
public:	
	UGenerateGrassComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void StartGenerateWorld(bool bEditor = false) {} ;
	virtual void NewGenerateWorld(FIntPoint SectionID, const TArray<FVector>& Vertices, const TArray<FVector2D>& UVs, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FProcMeshTangent>& Tangents) override{};
	virtual void DelGenerateWorld(FIntPoint SectionID, const TArray<FVector>& Vertices, const TArray<FVector2D>& UVs, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FProcMeshTangent>& Tangents) override {};
	virtual void FinishGenerateWorld() {};
};
