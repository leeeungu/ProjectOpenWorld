#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "BuildingPreviewComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTOPENWORLD_API UBuildingPreviewComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
protected:
	// 프리뷰용 기본 머티리얼 에셋
	UPROPERTY()
	TObjectPtr<UMaterial> buildingPreviewMat{};

	// 프리뷰용 동적 머티리얼 (Buildable 파라미터 제어)
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> buildingPreview = nullptr;

	// 현재 프리뷰에 사용 중인 StaticMesh
	UPROPERTY()
	TObjectPtr<UStaticMesh> TargetBuildingMesh{};

	// 현재 위치가 배치 가능(true) / 불가(false) 상태인지
	UPROPERTY()
	bool bBuildable = false;

public:
	UBuildingPreviewComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport) override;

	// 프리뷰 on/off
	void StartPreView();
	void EndPreView();

	// 건설 대상 StaticMesh 설정 (프리뷰용 메쉬 + Bounds용)
	void SetBuildingMsh(UStaticMesh* NewMesh);

	// Assist 쪽에서 배치 가능 여부를 조회할 때 사용
	bool IsBuildable() const { return bBuildable; }

protected:
	// 프리뷰 머티리얼 인스턴스 설정
	void SetBuildable(bool bValue);
};
