#pragma once

#include "CoreMinimal.h"
#include "Landscape/Component/GenerateWorldComponent.h"
#include "GemerateStaticObjectComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTOPENWORLD_API UGemerateStaticObjectComponent : public UGenerateWorldComponent
{
	GENERATED_BODY()

public:
	UGemerateStaticObjectComponent();

protected:
	virtual void BeginPlay() override;

	virtual void StartGenerateWorld(bool bEditor = false) {};
	virtual void NewGenerateWorld(FIntPoint SectionID, const TArray<FVector>& Vertices, const TArray<FVector2D>& UVs, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FProcMeshTangent>& Tangents) {}
	virtual void DelGenerateWorld(FIntPoint SectionID, const TArray<FVector>& Vertices, const TArray<FVector2D>& UVs, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FProcMeshTangent>& Tangents) {}
	virtual void FinishGenerateWorld() {};
public:
	virtual void Initialize(USceneComponent* ParentComponent) {};
};
