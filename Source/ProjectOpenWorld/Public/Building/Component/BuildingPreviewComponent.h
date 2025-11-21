#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "../DataTable/SnapRule.h"
#include "BuildingPreviewComponent.generated.h"


class UMaterial;
class UMaterialInstanceDynamic; 

UCLASS(ClassGroup = (Architecture), meta = (BlueprintSpawnableComponent))
class PROJECTOPENWORLD_API UBuildingPreviewComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
	TArray<FSnapRule*> ArrData{};
protected:
	TSet<const FSnapRule*> SnapParentSet{};
	TArray<const FSnapRule*> SnapParentArray{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BuildingPreview")
	TObjectPtr < UMaterial> buildingPreviewMat{};
	UPROPERTY()
	TObjectPtr <UMaterialInstanceDynamic> buildingPreview{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BuildingPreview")
	TObjectPtr < UStaticMesh> TargetBuildingMesh{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BuildingPreview")
	TObjectPtr < UStaticMeshComponent> ParentMesh{};

	FHitResult HittedResult{};
	FVector HitLocation{};
	bool bBuildable{};
	bool bSnapped{};
public:
	UBuildingPreviewComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
protected:
	virtual void BeginPlay() override;

public:

	UFUNCTION(BlueprintCallable, Category = "BuildingPreview")
	void StartPreView();
	UFUNCTION(BlueprintCallable, Category = "BuildingPreview")
	void EndPreView();
	UFUNCTION(BlueprintCallable, Category = "BuildingPreview")
	void SetParentMesh(FHitResult& Hit, UStaticMeshComponent* NewMesh);
	UFUNCTION(BlueprintCallable, Category = "BuildingPreview")
	void SetBuildingMsh(UStaticMesh* NewMesh);
	UFUNCTION(BlueprintPure, Category = "BuildingPreview")
	UStaticMesh* GetBuildingMsh() const { return TargetBuildingMesh.Get(); }
	UFUNCTION(BlueprintCallable, Category = "BuildingPreview")
	void SetBuildable(bool bValue);
	UFUNCTION(BlueprintPure, Category = "BuildingPreview")
	FORCEINLINE bool IsBuildable() const { return bBuildable; }

	virtual void OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport = ETeleportType::None) override;
};
