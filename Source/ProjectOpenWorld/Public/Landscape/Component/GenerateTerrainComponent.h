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
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	//TObjectPtr<UProceduralMeshComponent> TargetTerrain{}; 
	// 관리상의 문제로 더블 스왑 폐기
	// radius ^2 을 계속 업데이트 했는데 foliage할 때 너무 단점이 커서
	// 갱신 삭제만 으로 로직 변경 그 과정에서 더블 스왑은 오히려 복잡성만 증가 시킴

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	TObjectPtr<UProceduralMeshComponent> GenerateTerrain{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	TObjectPtr< UMaterialInterface> TerrainMaterial{};

	TMap<FIntPoint, int32> SectionIDToMeshIndex{};
	TArray<int32> EmptySectionIndex{};
	TArray<int32> UpdateSectionIndex{};
	int SectionIndex = 0;
public:	
	UGenerateTerrainComponent();
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
protected:
	virtual void BeginPlay() override;

	virtual void StartGenerateWorld(bool bEditor = false) override;
	virtual void NewGenerateWorld(FIntPoint SectionID, const TArray<FVector>& Vertices, const TArray<FVector2D>& UVs, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FProcMeshTangent>& Tangents) override;
	virtual void DelGenerateWorld(FIntPoint SectionID, const TArray<FVector>& Vertices, const TArray<FVector2D>& UVs, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FProcMeshTangent>& Tangents) override;
	virtual void FinishGenerateWorld() override;

public:
	virtual void Initialize(USceneComponent* ParentComponent) override;

	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
