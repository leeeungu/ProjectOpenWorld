#pragma once

#include "CoreMinimal.h"
#include "Landscape/Component/GenerateWorldComponent.h"
#include "GenerateTerrainComponent.generated.h"

struct FProcMeshTangent;
class UProceduralMeshComponent;

//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
//TObjectPtr<UProceduralMeshComponent> TargetTerrain{}; 
// 관리상의 문제로 더블 스왑 폐기
// radius ^2 을 계속 업데이트 했는데 foliage할 때 너무 단점이 커서
// 갱신 삭제만 으로 로직 변경 그 과정에서 더블 스왑은 오히려 복잡성만 증가 시킴

//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
//TObjectPtr<UProceduralMeshComponent> GenerateTerrain{};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UGenerateTerrainComponent : public UGenerateWorldComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings", meta = (ClampMin = "1", ClampMax = "200"))
	int32 TerrainComponentSize{ 36 };
	UPROPERTY(VisibleInstanceOnly)
	TArray<TObjectPtr<UProceduralMeshComponent>> GenerateTerrainArray{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	TObjectPtr< UMaterialInterface> TerrainMaterial{};

	TMap<FIntPoint, TObjectPtr<UProceduralMeshComponent>> SectionIDToMeshIndex{};
	TArray<TObjectPtr<UProceduralMeshComponent>> EmpthyMeshComponent{};
public:	
	UGenerateTerrainComponent();
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
protected:
	virtual void BeginPlay() override;

	virtual void StartGenerateWorld(bool bEditor = false) override;
	virtual void NewGenerateWorld(const FGenerateSectionData& SectionData) override;
	virtual void DelGenerateWorld(const FGenerateSectionData& SectionData) override;
	virtual void FinishGenerateWorld() override;

public:
	virtual void Initialize(USceneComponent* ParentComponent) override;
};
