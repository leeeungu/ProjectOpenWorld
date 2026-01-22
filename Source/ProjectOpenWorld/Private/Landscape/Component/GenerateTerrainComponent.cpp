#include "Landscape/Component/GenerateTerrainComponent.h"
#include "ProceduralMeshComponent.h"
#include "NavigationSystem.h"

UGenerateTerrainComponent::UGenerateTerrainComponent() : Super()
{
	PrimaryComponentTick.bCanEverTick = false;
	GenerateTerrainArray.Empty(false);
	EmpthyMeshComponent.Empty(false);
	GenerateTerrainArray.Reserve(TerrainComponentSize);
	EmpthyMeshComponent.Reserve(TerrainComponentSize);
	for (int i = 0; i < TerrainComponentSize; i++)
	{
		UProceduralMeshComponent* Mehs = CreateDefaultSubobject<UProceduralMeshComponent>(FName(*FString::Printf(TEXT("ProceduralMeshComponent_%d"), i)));
		//Cast< UProceduralMeshComponent>(GetOwner()->AddComponentByClass(UProceduralMeshComponent::StaticClass(), true, FTransform{}, false));
		//Mehs->SetupAttachment(GetOwner()->GetRootComponent());
		GenerateTerrainArray.Add(Mehs);
		EmpthyMeshComponent.Add(Mehs);
	}
	//GenerateTerrain =
}

void UGenerateTerrainComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = PropertyChangedEvent.GetPropertyName();
	if(PropertyName == GET_MEMBER_NAME_CHECKED(UGenerateTerrainComponent, TerrainMaterial))
	{
		for (UProceduralMeshComponent* GenerateTerrain : GenerateTerrainArray)
		{
			GenerateTerrain->SetMaterial(0, TerrainMaterial);
		}
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UGenerateTerrainComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UGenerateTerrainComponent::StartGenerateWorld(bool bEditor)
{
	if (bEditor)
	{
		SectionIDToMeshIndex.Empty();

		EmpthyMeshComponent.Empty(false);
		EmpthyMeshComponent.Reserve(TerrainComponentSize);
		for (UProceduralMeshComponent* GenerateTerrain : GenerateTerrainArray)
		{
			EmpthyMeshComponent.Add(GenerateTerrain);
		}
	}
}

void UGenerateTerrainComponent::NewGenerateWorld(const FGenerateSectionData& SectionData)
{
	TObjectPtr<UProceduralMeshComponent>& GenerateTerrain = SectionIDToMeshIndex.FindOrAdd(SectionData.SectionID, nullptr);
	if (!GenerateTerrain && !EmpthyMeshComponent.IsEmpty())
	{
		GenerateTerrain = EmpthyMeshComponent.Last();
		EmpthyMeshComponent.Pop();
		//UE_LOG(LogTemp, Warning, TEXT("UGenerateTerrainComponent New Mesh SectionID:(%d,%d)"), SectionID.X, SectionID.Y);
		GenerateTerrain->CreateMeshSection(0, *SectionData.Vertices, *SectionData.Triangles, *SectionData.Normals, *SectionData.UVs, TArray<FColor>(),* SectionData.Tangents, true);
		UNavigationSystemV1::UpdateComponentInNavOctree(*GenerateTerrain);
		if (TerrainMaterial)
		{
			GenerateTerrain->SetMaterial(0, TerrainMaterial);
		}
	}
}

void UGenerateTerrainComponent::DelGenerateWorld(const FGenerateSectionData& SectionData)
{
	if (TObjectPtr<UProceduralMeshComponent>* GenerateTerrain = SectionIDToMeshIndex.Find(SectionData.SectionID))
	{
		if (*GenerateTerrain)
		{
			(*GenerateTerrain)->ClearAllMeshSections();
			EmpthyMeshComponent.Push(*GenerateTerrain);
		}
		SectionIDToMeshIndex.Remove(SectionData.SectionID);
	}
}

void UGenerateTerrainComponent::FinishGenerateWorld()
{
	/*if (GenerateTerrain)
	{
		UNavigationSystemV1::UpdateComponentInNavOctree(*GenerateTerrain);
		for (int32 SectionID : UpdateSectionIndex)
		{
			GenerateTerrain->SetMeshSectionVisible(SectionID, true);
		}
		UpdateSectionIndex.Empty();
	}*/
}

void UGenerateTerrainComponent::Initialize(USceneComponent* ParentComponent)
{
	if (ParentComponent)
	{
		for (UProceduralMeshComponent* GenerateTerrain : GenerateTerrainArray)
		{
			GenerateTerrain->SetupAttachment(ParentComponent);
		}
	}
}


