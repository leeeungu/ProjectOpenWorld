#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Building/Interface/ArchitectureInterface.h"
#include "PalBuildingStaticMeshComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBuildingEnd);

UCLASS(ClassGroup = (Architecture), meta = (BlueprintSpawnableComponent))
class PROJECTOPENWORLD_API UPalBuildingStaticMeshComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
protected:
	TSoftObjectPtr < UMaterial> buildingMakingMat{};
	TArray<TSoftObjectPtr <UMaterialInstanceDynamic>> buildingMaking{};
	UPROPERTY(VisibleAnywhere, Category = "Building")
	float curentPercent = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building")
	float buildingTime = 3.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building")
	float buildSpeed = 1.0f;
	bool isBuilding = false;
	UPROPERTY(VisibleAnywhere, Category = "Building")
	TSet< TWeakObjectPtr<UObject>> InstigatorList{};

public:
	UPROPERTY(BlueprintAssignable, Category = "Building")
	FOnBuildingEnd onBuildingEnd{};
public:
	UPalBuildingStaticMeshComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginPlay() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UFUNCTION()
	void EndBuilding();
public:
	//UFUNCTION(BlueprintCallable, Category = "Building")
	virtual bool SetStaticMesh(class UStaticMesh* NewMesh) override;
	//void SetbuildingMesh(UStaticMesh* NewMesh);

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

	//FORCEINLINE UStaticMeshComponent* GetMeshComponent() const;
private:
	void SetBuildingPercent(float Value);
};