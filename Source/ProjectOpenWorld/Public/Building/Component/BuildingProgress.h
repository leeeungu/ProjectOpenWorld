// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuildingProgress.generated.h"

class UStaticMeshComponent;
class UStaticMesh;
class UMaterial;
class UMaterialInstanceDynamic;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UBuildingProgress : public UActorComponent
{
	GENERATED_BODY()
protected:
	// 해당 기능을 묶어서 처리면 좋을듯?
	TSoftObjectPtr<UStaticMeshComponent> buildingMeshComponent{};
	TSoftObjectPtr < UMaterial> buildingMakingMat{};
	TArray<TSoftObjectPtr <UMaterialInstanceDynamic>> buildingMaking{};
	float curentPercent = 0.3f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building")
	TSoftObjectPtr<UStaticMesh> buildingMesh{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building")
	float buildingTime = 3.0f;

public:
	UBuildingProgress();


	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginPlay() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
public:
	UFUNCTION(BlueprintCallable, Category = "Building")
	void SetbuildingMesh(UStaticMesh* NewMesh);

	UFUNCTION(BlueprintCallable, Category = "Building")
	void StartBuilding();
	UFUNCTION(BlueprintCallable, Category = "Building")
	void StopBuilding();
	UFUNCTION(BlueprintCallable, Category = "Building")
	void ResumeBuilding();
	UFUNCTION(BlueprintCallable, Category = "Building")
	void EndBuilding();

	bool IsAlreadyStart() const { return !FMath::IsNearlyZero(curentPercent); }
protected:
	FORCEINLINE UStaticMeshComponent* GetMeshComponent() const { return buildingMeshComponent.Get(); }
};
