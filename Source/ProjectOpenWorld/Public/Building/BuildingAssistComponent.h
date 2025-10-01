
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
	TSoftObjectPtr < APawn> ownerPawn{};

	bool buildingActive{};

public: // ActorComponent Function	
	UBuildingAssistComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

public: // Custom Function
	UFUNCTION(BlueprintCallable, Category = "BuildingAssist")
	void SetBuildingStaticMesh(UStaticMesh* NewStaticMesh);
	UFUNCTION(BlueprintCallable, Category = "BuildingAssist")
	void SpawnBuilding();
private:
	void OnOffAssist(bool bValue);
};
