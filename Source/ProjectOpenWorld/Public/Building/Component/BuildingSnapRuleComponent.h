#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuildingSnapRuleComponent.generated.h"

class UBuildingPreviewComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTOPENWORLD_API UBuildingSnapRuleComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	TObjectPtr<UBuildingPreviewComponent> buildingPreviewActor{};

	UPROPERTY()
	TSoftObjectPtr < APawn> ownerPawn{};

public:
	UBuildingSnapRuleComponent();

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
protected:
	bool bSnapped{};

	//	UPROPERTY(EditDefaultsOnly, Category = "BuildingWidget", BlueprintReadOnly)
	//	TSubclassOf<UBuildingInfoWidget> BuildingInfoClass{};
	//	UBuildingInfoWidget* BuildingInfo{};
	//	UPROPERTY(EditDefaultsOnly, Category = "Building", BlueprintReadOnly)
	//	TSubclassOf<ABaseBuilding> BuildingClass{};
	//	UPROPERTY(VisibleAnywhere)
	//	TSoftObjectPtr < AActor> targetActor{};
	//	TMap<FName, FTransform> snapSocketTransform{};
	//	TArray<TPair< UStaticMeshComponent*, FName>> ArraySnapSocket{};
	//	FName snapSocketName{};
	//	FVector meshSize{};
	//	FVector meshCenter{};
	//
	//	TArray<TEnumAsByte<EObjectTypeQuery> > buildPointObjectTypes{};
	//	TArray<AActor*> buildPointIgnore{};
	//	bool FindBestSnapTransform(
	//		UStaticMeshComponent* ParentComp,
	//		const FVector& HitWorldLocation,
	//		FTransform& OutChildWorldTransform) const;
	//protected:
	//	virtual void BeginPlay() override;
	//
	//public: // Custom Function
	//	UFUNCTION(BlueprintCallable, Category = "BuildingAssist")
	//	void SetBuildingStaticMesh(UStaticMesh* NewStaticMesh);
	//	UFUNCTION(BlueprintCallable, Category = "BuildingAssist")
	//	void StartBuilding();
	//	UFUNCTION(BlueprintCallable, Category = "BuildingAssist")
	//	void EndBuilding();
	//	UFUNCTION(BlueprintCallable, Category = "BuildingAssist")
	//	void SpawnBuilding();
	//	UFUNCTION(BlueprintCallable, Category = "BuildingAssist")
	//	void RotateBuilding(float AddYaw);
	//	UFUNCTION(BlueprintPure, Category = "BuildingAssist")
	//	bool IsBuildingActive() const { return buildingActive; }
	//
	//	UFUNCTION(BlueprintPure, Category = "BuildingAssist")
	//	AActor* GetTargetActor() { return targetActor.Get(); }
	//private:
	//	void OnOffAssist(bool bValue);
	//	bool UpdateTraceHit(FHitResult& HitResult);
	//	bool UpdateSnap(FVector& ResultPoint);
	//	bool UpdatePreviewMesh(FVector& ResultPoint);
	//
	//	bool UpdateBuildable();
	//	void UpdatePreviewMat();
	//};

};