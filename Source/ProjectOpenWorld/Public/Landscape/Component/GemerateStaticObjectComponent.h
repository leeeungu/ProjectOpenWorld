#pragma once

#include "CoreMinimal.h"
#include "Landscape/Component/GenerateWorldComponent.h"
#include "GemerateStaticObjectComponent.generated.h"

class IGenerateWorldInterface;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTOPENWORLD_API UGemerateStaticObjectComponent : public UGenerateWorldComponent
{
	GENERATED_BODY()

protected:
	TMap < FIntPoint, TSet < TScriptInterface <IGenerateWorldInterface >> > SectionStaticObjectMap{};
public:
	UGemerateStaticObjectComponent();

protected:
	virtual void BeginPlay() override;

	virtual void StartGenerateWorld(bool bEditor = false) {};
	virtual void NewGenerateWorld(const FGenerateSectionData& SectionData) override;
	virtual void DelGenerateWorld(const FGenerateSectionData& SectionData) override;
	virtual void FinishGenerateWorld() {};
	UFUNCTION()
	void CommandActorSpawned(AActor* SpawnActor);

public:
	virtual void Initialize(USceneComponent* ParentComponent) {};
};
