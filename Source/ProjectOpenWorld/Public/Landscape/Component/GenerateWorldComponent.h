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
	bool bStartGenerate{ false };
	//virtual void BeginPlay() override;
	UPROPERTY()
	TObjectPtr <UGeneratorSectionComponent> GeneratorSectionComponent{};

	UFUNCTION()
	virtual void StartGenerateWorld(bool bEditor = false) { bStartGenerate = true; };
	UFUNCTION()
	virtual void NewGenerateWorld(const FGenerateSectionData& SectionData) {}
	UFUNCTION()
	virtual void DelGenerateWorld(const FGenerateSectionData& SectionData) {}
	UFUNCTION()
	virtual void FinishGenerateWorld() { bStartGenerate = false; };

	void SetGeneratorSectionComponent(UGeneratorSectionComponent* GeneratorComponent) { 
		GeneratorSectionComponent = GeneratorComponent; 
	}
private:
	void StartGenerate() { bStartGenerate = true; }
	void EndGenerate() { bStartGenerate = false; }
protected:
	bool IsGenerating() const { return bStartGenerate; }
public:	
	virtual void Initialize(USceneComponent* ParentComponent) {};
};
