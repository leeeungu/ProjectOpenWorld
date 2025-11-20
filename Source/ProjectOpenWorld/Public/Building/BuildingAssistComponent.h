#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Building/Component/BuildingPreviewComponent.h"
#include "BuildingAssistComponent.generated.h"

class AStaticMeshActor;
class UBuildingInfoWidget;
class ABaseBuilding;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UBuildingAssistComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	TObjectPtr<UBuildingPreviewComponent> buildingPreviewActor{};
	UPROPERTY()
	TSoftObjectPtr < APawn> ownerPawn{};
	TSoftObjectPtr < UStaticMesh> BuildingMesh{};
	
	bool buildingActive{};
	bool canBuilding{};
	bool bSnapped{};

	UPROPERTY(EditDefaultsOnly, Category = "BuildingWidget", BlueprintReadOnly)
	TSubclassOf<UBuildingInfoWidget> BuildingInfoClass{};
	UBuildingInfoWidget* BuildingInfo{};
	UPROPERTY(EditDefaultsOnly, Category = "Building", BlueprintReadOnly)
	TSubclassOf<ABaseBuilding> BuildingClass{};
	UPROPERTY(VisibleAnywhere)
	TSoftObjectPtr < AActor> targetActor{};
	TMap<FName, FTransform> snapSocketTransform{};
	TArray<TPair< UStaticMeshComponent*, FName>> ArraySnapSocket{};
	FName snapSocketName{};
	FVector meshSize{};
	FVector meshCenter{};

	TArray<TEnumAsByte<EObjectTypeQuery> > buildPointObjectTypes{};
	TArray<TEnumAsByte<EObjectTypeQuery> > buildCheckObjectTypes{};
	TArray<AActor*> buildPointIgnore{};
	TArray<AActor*> buildCheckIgnore{};
public: // ActorComponent Function	
	UBuildingAssistComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
protected:
	virtual void BeginPlay() override;

public: // Custom Function
	UFUNCTION(BlueprintCallable, Category = "BuildingAssist")
	void SetBuildingStaticMesh(UStaticMesh* NewStaticMesh);
	UFUNCTION(BlueprintCallable, Category = "BuildingAssist")
	void StartBuilding();
	UFUNCTION(BlueprintCallable, Category = "BuildingAssist")
	void EndBuilding();
	UFUNCTION(BlueprintCallable, Category = "BuildingAssist")
	void SpawnBuilding();
	UFUNCTION(BlueprintCallable, Category = "BuildingAssist")
	void RotateBuilding(float AddYaw);
	UFUNCTION(BlueprintPure, Category = "BuildingAssist")
	bool IsBuildingActive() const { return buildingActive; }

	UFUNCTION(BlueprintPure, Category = "BuildingAssist")
	AActor* GetTargetActor() { return targetActor.Get(); }
private:
	void OnOffAssist(bool bValue);
	bool UpdateTraceHit(FHitResult& HitResult);
	bool UpdateSnap(FVector& ResultPoint);
	bool UpdatePreviewMesh(FVector& ResultPoint);

	bool UpdateBuildable();
	void UpdatePreviewMat();
};
