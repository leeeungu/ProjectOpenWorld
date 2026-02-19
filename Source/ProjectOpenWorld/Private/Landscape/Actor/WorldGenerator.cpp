#include "Landscape/Actor/WorldGenerator.h"
#include "Landscape/Component/GeneratorSectionComponent.h"
#include "Landscape/Component/GenerateTerrainComponent.h"
#include "Landscape/Component/GenerateFoliageComponent.h"
#include "Landscape/Component/GemerateStaticObjectComponent.h"

AWorldGenerator::AWorldGenerator() : AActor{}
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	
	SectionGenerator = CreateDefaultSubobject<UGeneratorSectionComponent>(TEXT("SectionGenerator"));

	TerrainGenerator = CreateDefaultSubobject<UGenerateTerrainComponent>(TEXT("TerrainGenerator"));
	SectionGenerator->BindGenerteComponent(TerrainGenerator);
	TerrainGenerator->Initialize(RootComponent);

	FoliageGenerator = CreateDefaultSubobject<UGenerateFoliageComponent>(TEXT("FoliageGenerator"));
	SectionGenerator->BindGenerteComponent(FoliageGenerator);
	FoliageGenerator->Initialize(RootComponent);

	StaticObjectGenerator = CreateDefaultSubobject<UGemerateStaticObjectComponent>(TEXT("StaticObjectGenerator"));
	SectionGenerator->BindGenerteComponent(StaticObjectGenerator);
	StaticObjectGenerator->Initialize(RootComponent);
	Tags.Add(FName("Ladnscape"));
}

void AWorldGenerator::BeginPlay()
{
	Super::BeginPlay();
}
