#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseBuilding.generated.h"

class UStaticMesh;
class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class UMaterials;
class UBuildingProgress;


UCLASS(BlueprintType, Blueprintable)
class PROJECTOPENWORLD_API ABaseBuilding : public AActor
{
	GENERATED_BODY()
protected:
	// 해당 기능을 묶어서 처리면 좋을듯?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building", BlueprintGetter = "GetBuildingMeshComponent")
	UStaticMeshComponent* buildingMeshComponent{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building", BlueprintGetter = "GetBuildingProgress")
	UBuildingProgress* buildingProgressComponent{};
	
public:		
	ABaseBuilding();

	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintPure, Category = "Building")
	FORCEINLINE UStaticMeshComponent* GetBuildingMeshComponent() const { return buildingMeshComponent; }
	UFUNCTION(BlueprintPure, Category = "Building")
	FORCEINLINE UBuildingProgress* GetBuildingProgress() const { return buildingProgressComponent; }
};
