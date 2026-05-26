#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldGenerator.generated.h"

class UInstancedStaticMeshComponent;
class UGeneratorSectionComponent;
class UGenerateTerrainComponent;
class UGenerateFoliageComponent;
class UGemerateStaticObjectComponent;
class UGenerateSpawnerComponent;

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	TObjectPtr<UGenerateSpawnerComponent> SpawnerGenerator{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Landscape Settings")
	TObjectPtr<UGemerateStaticObjectComponent> StaticObjectGenerator{};
	
public:	
	AWorldGenerator();

protected:
	virtual void BeginPlay() override;
	
public:	
	FORCEINLINE UGeneratorSectionComponent* GetGeneratorSectionComponent() const { return SectionGenerator; }
	//virtual void Tick(float DeltaTime) override;
};

