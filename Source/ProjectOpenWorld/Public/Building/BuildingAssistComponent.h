
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuildingAssistComponent.generated.h"

class AStaticMeshActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UBuildingAssistComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	TSoftObjectPtr<AStaticMeshActor> buildingPreviewActor{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BuildingAssist")
	TSoftObjectPtr < UMaterial> buildingPreviewMat{};

	UPROPERTY()
	TSoftObjectPtr <UMaterialInstanceDynamic> buildingPreview{};
	UPROPERTY()
	TSoftObjectPtr < APawn> ownerPawn{};

	bool buildingActive{};
	bool canBuilding{};

	UPROPERTY()
	TSoftObjectPtr < AActor> targetActor{};
	TMap<FName, FTransform> snapSocketTransform{};
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
private:
	void OnOffAssist(bool bValue);
	bool UpdateTraceHit(FHitResult& HitResult);
	bool UpdateSnap(FVector& ResultPoint);
	bool UpdateBuildable();
	void UpdatePreviewMat();
};
