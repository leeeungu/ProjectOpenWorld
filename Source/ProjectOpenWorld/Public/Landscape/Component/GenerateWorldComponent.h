#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Landscape/Component/GeneratorSectionComponent.h"
#include "GenerateWorldComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) ,Abstract)
class PROJECTOPENWORLD_API UGenerateWorldComponent : public UActorComponent
{
	GENERATED_BODY()
	friend UGeneratorSectionComponent;

protected:
	//virtual void BeginPlay() override;
	UPROPERTY()
	TObjectPtr <UGeneratorSectionComponent> GeneratorSectionComponent{};

	UFUNCTION()
	virtual void StartGenerateWorld(bool bEditor = false) {};
	UFUNCTION()
	virtual void NewGenerateWorld(const FGenerateSectionData& SectionData) {}
	UFUNCTION()
	virtual void DelGenerateWorld(const FGenerateSectionData& SectionData) {}
	UFUNCTION()
	virtual void FinishGenerateWorld() {};

	void SetGeneratorSectionComponent(UGeneratorSectionComponent* GeneratorComponent) { 
		GeneratorSectionComponent = GeneratorComponent; 
	}
public:	
	virtual void Initialize(USceneComponent* ParentComponent) {};
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
