#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldGenerator.generated.h"

class UInstancedStaticMeshComponent;
class UGeneratorSectionComponent;
class UGenerateTerrainComponent;
class UGenerateFoliageComponent;

UCLASS()
class PROJECTOPENWORLD_API AWorldGenerator : public AActor
{
	//friend class UQuadtreeWorldSubsystem;
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	TObjectPtr<UGeneratorSectionComponent> SectionGenerator{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	TObjectPtr<UGenerateTerrainComponent> TerrainGenerator{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	TObjectPtr<UInstancedStaticMeshComponent> TreeInstancedMeshComponent{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	TObjectPtr<UGenerateFoliageComponent> FoliageGenerator{};
	
	
public:	
	AWorldGenerator();

protected:
	virtual void BeginPlay() override;
	
public:	
	FORCEINLINE UGeneratorSectionComponent* GetGeneratorSectionComponent() const { return SectionGenerator; }
	//virtual void Tick(float DeltaTime) override;
};

