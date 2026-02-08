// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Building/Interface/ArchitectureInterface.h"
#include "BuildingProgress.generated.h"

class UStaticMeshComponent;
class UStaticMesh;
class UMaterial;
class UMaterialInstanceDynamic;
class IArchitectureInterface;

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBuildingEnd);

UCLASS( ClassGroup=(Architecture), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UBuildingProgress : public UActorComponent
{
	GENERATED_BODY()
protected:
	// 해당 기능을 묶어서 처리면 좋을듯?
	TSoftObjectPtr<UStaticMeshComponent> buildingMeshComponent{};
	TSoftObjectPtr < UMaterial> buildingMakingMat{};
	TArray<TSoftObjectPtr <UMaterialInstanceDynamic>> buildingMaking{};
	float curentPercent = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building")
	TSoftObjectPtr<UStaticMesh> buildingMesh{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building")
	float buildingTime = 3.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building")
	float buildSpeed = 1.0f;
	bool isBuilding = false;
	UPROPERTY(VisibleAnywhere)
	TSet< TWeakObjectPtr<UObject>> InstigatorList{};

public:
	//UPROPERTY(BlueprintAssignable, Category = "Building")
	//FOnBuildingEnd onBuildingEnd{};
public:
	UBuildingProgress();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginPlay() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UFUNCTION()
	void EndBuilding();
public:
	UFUNCTION(BlueprintCallable, Category = "Building")
	void SetbuildingMesh(UStaticMesh* NewMesh);

	UFUNCTION(BlueprintCallable, Category = "Building")
	void StartBuilding(TScriptInterface<IArchitectureInterface> OtherInstigator);
	UFUNCTION(BlueprintCallable, Category = "Building")
	void StopBuilding(TScriptInterface<IArchitectureInterface> OtherInstigator);

	UFUNCTION(BlueprintCallable, Category = "Building")
	void StopAll();

	bool IsAlreadyStart() const { return !FMath::IsNearlyZero(curentPercent); }
	UFUNCTION(BlueprintPure, Category = "Building")
	bool IsBuildingEnd() const;

	void ActiveBuildingNav();
	void DeActiveBuildingNav();
	const float* GetBuildPercent() const { return &curentPercent; }
	const float* GetBuildTime() const { return &buildingTime; }
	const float* GetBuildSpeed() const { return &buildSpeed; }
	
	FORCEINLINE UStaticMeshComponent* GetMeshComponent() const { return buildingMeshComponent.Get(); }
private:
	void SetBuildingPercent(float Value);
};
