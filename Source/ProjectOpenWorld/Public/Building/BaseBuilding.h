#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseBuilding.generated.h"

class UStaticMesh;
class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class UMaterials;

UCLASS(Blueprintable)
class PROJECTOPENWORLD_API ABaseBuilding : public AActor
{
	GENERATED_BODY()
protected:
	// 해당 기능을 묶어서 처리면 좋을듯?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building")
	TSoftObjectPtr<UStaticMeshComponent> buildingMeshComponent{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
	TSoftObjectPtr<UStaticMesh> buildingMesh{};
	TSoftObjectPtr < UMaterial> buildingMakingMat{};
	TArray<TSoftObjectPtr <UMaterialInstanceDynamic>> buildingMaking{};
	float curentPercent = 0.3f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
	float buildingTime = 3.0f;
public:	
	ABaseBuilding();

	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
public:
	UFUNCTION(BlueprintCallable, Category = "Building")
	void SetbuildingMesh(UStaticMesh* NewMesh);

	UFUNCTION(BlueprintCallable, Category = "Building")
	void StartBuilding();
	UFUNCTION(BlueprintCallable, Category = "Building")
	void StopBuilding();
	UFUNCTION(BlueprintCallable, Category = "Building")
	void EndBuilding();
protected:
	virtual void BeginPlay() override;
	FORCEINLINE UStaticMeshComponent* GetMeshComponent() const { return buildingMeshComponent.Get(); }
};
