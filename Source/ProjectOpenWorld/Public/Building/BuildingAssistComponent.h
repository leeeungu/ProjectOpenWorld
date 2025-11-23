#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Building/DataTable/SnapRule.h"
#include "BuildingAssistComponent.generated.h"

class APawn;
class ABaseBuilding;
class UStaticMesh;
class UBuildingPreviewComponent;
class UBuildingInfoWidget;
struct FSnapRule;

/**
 * 플레이어 빌딩 보조 컴포넌트.
 * - 카메라 방향 Trace
 * - 스냅 위치 계산
 * - PreviewComponent Transform 이동
 * - PreviewComponent의 겹침 판정 결과를 읽어 canBuilding 설정
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTOPENWORLD_API UBuildingAssistComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	// 소유 Pawn
	UPROPERTY()
	TWeakObjectPtr<APawn> ownerPawn;

	// 프리뷰용 컴포넌트
	UPROPERTY()
	UBuildingPreviewComponent* buildingPreviewActor = nullptr;

	// 스폰할 빌딩 클래스
	UPROPERTY(EditAnywhere, Category = "Building")
	TSubclassOf<ABaseBuilding> BuildingClass;

	// 현재 선택된 건설 StaticMesh
	UPROPERTY()
	TWeakObjectPtr<UStaticMesh> BuildingMesh;

	// 빌딩 모드 활성 여부
	UPROPERTY()
	bool buildingActive = false;

	// 현재 위치에서 실제 건설 가능 여부
	UPROPERTY(VisibleAnywhere, Category = "Building")
	bool canBuilding = false;

	// 카메라 Trace용 ObjectTypes / IgnoreActors
	UPROPERTY()
	TArray<TEnumAsByte<EObjectTypeQuery>> buildPointObjectTypes;

	UPROPERTY()
	TArray<AActor*> buildPointIgnore;

	// UI
	UPROPERTY()
	TSubclassOf<UUserWidget> BuildingInfoClass;

	UPROPERTY()
	UBuildingInfoWidget* BuildingInfo = nullptr;

	// 스냅 데이터 (DataTable에서 로드된 전체 Row 포인터)
	TArray<FSnapRule*> SnapDataRows{};

	// 현재 ChildMesh(=BuildingMesh)에 대해 유효한 모든 룰
	TArray<const FSnapRule*> SnapRulesForChild;

	// Snap 거리 임계값(ImpactPoint와 ParentAnchorWorld 사이 거리)
	UPROPERTY(EditAnywhere, Category = "Building|Snap")
	float SnapDistance = 100.f;

	float YawRotation = 0.f;
public:
	UBuildingAssistComponent();

protected:
	virtual void BeginPlay() override;
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 외부에서 건설할 StaticMesh 선택
	UFUNCTION(BlueprintCallable, Category = "BuildingAssist")
	void SetBuildingStaticMesh(UStaticMesh* NewStaticMesh);

	// 빌딩 모드 On/Off
	UFUNCTION(BlueprintCallable, Category = "BuildingAssist")
	void StartBuilding();
	UFUNCTION(BlueprintCallable, Category = "BuildingAssist")
	void EndBuilding();

	// 실제 빌딩 Actor 스폰
	UFUNCTION(BlueprintCallable, Category = "BuildingAssist")
	void SpawnBuilding();

	// 프리뷰 회전
	UFUNCTION(BlueprintCallable, Category = "BuildingAssist")
	void RotateBuilding(float AddYaw);

protected:
	void OnOffAssist(bool bValue);

	// 카메라 기준 Trace + 스냅 위치 계산 + Preview 이동
	// true: 어떤 ParentMesh에 대해서든 스냅/자유배치 위치까지 처리 완료
	bool UpdatePreview();

};
