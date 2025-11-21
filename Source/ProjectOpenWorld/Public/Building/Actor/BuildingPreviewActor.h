#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BuildingPreviewActor.generated.h"

class UStaticMeshComponent;
class UMaterial;
class UMaterialInstanceDynamic;
class UStaticMeshSocket;

UCLASS(BlueprintType, Blueprintable, ClassGroup = Architecture)

class PROJECTOPENWORLD_API ABuildingPreviewActor : public AActor
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BuildingAssist")
	TSoftObjectPtr < UMaterial> buildingPreviewMat{};
	UPROPERTY()
	TSoftObjectPtr <UMaterialInstanceDynamic> buildingPreview{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BuildingAssist")
	TObjectPtr<UStaticMeshComponent> BuildingMesh{};

	TObjectPtr<UStaticMeshSocket> MeshSocket{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BuildingAssist")
	TSoftObjectPtr < UStaticMesh> TargetBuildingMesh{};

	bool bBuildable{};
public:	
	ABuildingPreviewActor();

protected:
	virtual void BeginPlay() override;

public:	

	UFUNCTION(BlueprintPure, Category = "BuildingAssist")
	FORCEINLINE UStaticMeshComponent* GetStaicMesh() const { return BuildingMesh; }
	UFUNCTION(BlueprintCallable, Category = "BuildingAssist")
	void SetBuildingMsh(UStaticMesh* NewMesh);

	UFUNCTION(BlueprintPure, Category = "BuildingAssist")
	UStaticMesh* GetBuildingMsh() const { return TargetBuildingMesh.Get(); }
	UFUNCTION(BlueprintCallable, Category = "BuildingAssist")
	void SetBuildable(bool bValue);
	UFUNCTION(BlueprintPure, Category = "BuildingAssist")
	FORCEINLINE bool IsBuildable() const { return bBuildable; }
};
