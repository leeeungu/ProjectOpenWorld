#include "Landscape/Component/GenerateTerrainComponent.h"
#include "ProceduralMeshComponent.h"
#include "NavigationSystem.h"

UGenerateTerrainComponent::UGenerateTerrainComponent() : Super()
{
	PrimaryComponentTick.bCanEverTick = false;
	GenerateTerrain = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GenerateTerrain"));
}

void UGenerateTerrainComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = PropertyChangedEvent.GetPropertyName();
	if(PropertyName == GET_MEMBER_NAME_CHECKED(UGenerateTerrainComponent, TerrainMaterial))
	{
		if(GenerateTerrain)
		{
			int32 SectionCount = GenerateTerrain->GetNumSections();
			for (int32 i = 0; i < SectionCount; ++i)
			{
				GenerateTerrain->SetMaterial(i, TerrainMaterial);
			}
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
		EmptySectionIndex.Empty();
		UpdateSectionIndex.Empty();
		SectionIndex = 0;
		GenerateTerrain->ClearAllMeshSections();
	}
}

void UGenerateTerrainComponent::NewGenerateWorld(FIntPoint SectionID, const TArray<FVector>& Vertices, const TArray<FVector2D>& UVs, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FProcMeshTangent>& Tangents)
{
	if (!GenerateTerrain)
		return;
	if (!SectionIDToMeshIndex.Find(SectionID))
	{
		//UE_LOG(LogTemp, Warning, TEXT("UGenerateTerrainComponent New Mesh SectionID:(%d,%d)"), SectionID.X, SectionID.Y);
		int32 CurrentMeshIndex = SectionIndex;
		if (!EmptySectionIndex.IsEmpty())
		{
			CurrentMeshIndex = EmptySectionIndex.Last();
			EmptySectionIndex.Pop();
		}
		else
			SectionIndex++;
		GenerateTerrain->CreateMeshSection(CurrentMeshIndex, Vertices, Triangles, Normals, UVs, TArray<FColor>(), Tangents, true);
		UpdateSectionIndex.Add(CurrentMeshIndex);
		if (TerrainMaterial)
		{
			GenerateTerrain->SetMaterial(CurrentMeshIndex, TerrainMaterial);
		}
		SectionIDToMeshIndex.Add(SectionID, CurrentMeshIndex);
	}
}

void UGenerateTerrainComponent::DelGenerateWorld(FIntPoint SectionID, const TArray<FVector>& Vertices, const TArray<FVector2D>& UVs, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FProcMeshTangent>& Tangents)
{
	if (int32* DelIndex = SectionIDToMeshIndex.Find(SectionID))
	{
		//UE_LOG(LogTemp, Warning, TEXT("UGenerateTerrainComponent Del Mesh SectionID:(%d,%d)"), SectionID.X, SectionID.Y);
		EmptySectionIndex.Add(*DelIndex);
		GenerateTerrain->SetMeshSectionVisible(*DelIndex, false);
		GenerateTerrain->ClearMeshSection(*DelIndex);
		SectionIDToMeshIndex.Remove(SectionID);
	}
}

void UGenerateTerrainComponent::FinishGenerateWorld()
{
	if (GenerateTerrain)
	{
		UNavigationSystemV1::UpdateComponentInNavOctree(*GenerateTerrain);
		for (int32 SectionID : UpdateSectionIndex)
		{
			GenerateTerrain->SetMeshSectionVisible(SectionID, true);
		}
		UpdateSectionIndex.Empty();
	}
}

void UGenerateTerrainComponent::Initialize(USceneComponent* ParentComponent)
{
	if (ParentComponent)
	{
		GenerateTerrain->SetupAttachment(ParentComponent);
	}
}


