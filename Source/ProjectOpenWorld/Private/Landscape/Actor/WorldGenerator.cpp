#include "Landscape/Actor/WorldGenerator.h"
#include "Landscape/Component/GeneratorSectionComponent.h"
#include "Landscape/Component/GenerateTerrainComponent.h"
#include "Landscape/Component/GenerateFoliageComponent.h"
#include "Landscape/Component/GemerateStaticObjectComponent.h"

AWorldGenerator::AWorldGenerator() : AActor{}
{
	PrimaryActorTick.bCanEverTick = true;
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
	////SectionGenerator->OnGenerateStart.AddDynamic(this, &AWorldGenerator::SetGeneratorMesh);
	//SectionGenerator->OnUpdateSection.AddUniqueDynamic(TerrainGenerator, &UGenerateTerrainComponent::UpdateMesh);
	//SectionGenerator->OnGenerateFinished.AddUniqueDynamic(TerrainGenerator, &UGenerateTerrainComponent::SetCurrentMesh);

	//GenerateTerrain->SetVisibility(false);
	//GenerateTerrain->SetActive(false);
	//GenerateTerrain->SetComponentTickEnabled(false);
	//SetGeneratorMesh();
	//UInstancedStaticMeshComponent* FoligeMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("FoligeMesh"));
	//FoligeMesh->SetupAttachment(RootComponent);
	//FoligeMesh->AddInstance(FTransform());
	//FoligeMesh->instance
		//FoligeMesh->ClearInstances();
}

void AWorldGenerator::BeginPlay()
{
	Super::BeginPlay();
}


//void AWorldGenerator::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//}