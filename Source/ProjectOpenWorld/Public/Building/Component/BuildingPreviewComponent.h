#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "BuildingPreviewComponent.generated.h"


class UMaterial;
class UMaterialInstanceDynamic; 

UCLASS(ClassGroup = (Architecture), meta = (BlueprintSpawnableComponent))
class PROJECTOPENWORLD_API UBuildingPreviewComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BuildingPreview")
	TSoftObjectPtr < UMaterial> buildingPreviewMat{};
	UPROPERTY()
	TSoftObjectPtr <UMaterialInstanceDynamic> buildingPreview{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BuildingPreview")
	TSoftObjectPtr < UStaticMesh> TargetBuildingMesh{};


	bool bBuildable{};
public:
	UBuildingPreviewComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;

public:

	UFUNCTION(BlueprintCallable, Category = "BuildingPreview")
	void StartPreView();
	UFUNCTION(BlueprintCallable, Category = "BuildingPreview")
	void EndPreView();

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
