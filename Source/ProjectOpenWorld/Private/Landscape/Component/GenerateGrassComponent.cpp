#include "Landscape/Component/GenerateGrassComponent.h"
#include "GrassInstancedStaticMeshComponent.h"
#include "LandscapeGrassType.h"

UGenerateGrassComponent::UGenerateGrassComponent() : UGenerateWorldComponent{}
{
	PrimaryComponentTick.bCanEverTick = true;
	GrassInstances.Init(nullptr, 5);

	// UHierarchicalInstancedStaticMeshComponent에서  ULandscapeGrassType사용하여  풀 만들기
}

void UGenerateGrassComponent::BeginPlay()
{
	Super::BeginPlay();
	for (int GrassID{}; GrassID < GrassTypes.Num(); GrassID++)
	{
		if (GrassInstances.IsValidIndex(GrassID))
		{
			UGrassInstancedStaticMeshComponent* Grass = Cast< UGrassInstancedStaticMeshComponent>(GetOwner()->AddComponentByClass(UGrassInstancedStaticMeshComponent::StaticClass(), true, FTransform{}, false));
			Grass->SetupAttachment(GetOwner()->GetRootComponent());
			Grass->SetStaticMesh(GrassTypes[GrassID]->GrassMesh_DEPRECATED);
			GrassInstances[GrassID] = Grass;
		}
	}
}

void UGenerateGrassComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UGenerateGrassComponent::NewGenerateWorld(const FGenerateSectionData& SectionData )
{
}

void UGenerateGrassComponent::DelGenerateWorld(const FGenerateSectionData& SectionData )
{
}
