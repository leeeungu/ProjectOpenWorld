#pragma once

#include "CoreMinimal.h"
#include "Landscape/Component/GenerateWorldComponent.h"
#include "GenerateGrassComponent.generated.h"

class ULandscapeGrassType;
class UGrassInstancedStaticMeshComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UGenerateGrassComponent : public UGenerateWorldComponent
{
	GENERATED_BODY()
protected:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings")
	TArray<TObjectPtr<ULandscapeGrassType>> GrassTypes{};
	UPROPERTY()//BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings")
	TArray<TObjectPtr< UGrassInstancedStaticMeshComponent>> GrassInstances{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings")
	float GrassRadius = 1.0f;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Landscape Settings")
	int32 GrassMaxCount = 10000;
public:	
	UGenerateGrassComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void StartGenerateWorld(bool bEditor = false) {} ;
	virtual void NewGenerateWorld(const FGenerateSectionData& SectionData) override;
	virtual void DelGenerateWorld(const FGenerateSectionData& SectionData) override;
	virtual void FinishGenerateWorld() {};
};
