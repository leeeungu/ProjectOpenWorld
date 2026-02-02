#include "Landscape/Component/GenerateTerrainComponent.h"
#include "ProceduralMeshComponent.h"
#include "NavigationSystem.h"

UGenerateTerrainComponent::UGenerateTerrainComponent() : Super()
{
	PrimaryComponentTick.bCanEverTick = false;
	GenerateTerrainArray.Empty(true);
	EmpthyMeshComponent.Empty(true);
	GenerateTerrainArray.Reserve(TerrainComponentSize);
	EmpthyMeshComponent.Reserve(TerrainComponentSize);
	for (int i = 0; i < TerrainComponentSize; i++)
	{
		UProceduralMeshComponent* Mehs = CreateDefaultSubobject<UProceduralMeshComponent>(FName(*FString::Printf(TEXT("ProceduralMeshComponent_%d"), i)));
		GenerateTerrainArray.Add(Mehs);
		EmpthyMeshComponent.Add(Mehs);
	}
}

void UGenerateTerrainComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = PropertyChangedEvent.GetPropertyName();
	if(PropertyName == GET_MEMBER_NAME_CHECKED(UGenerateTerrainComponent, TerrainMaterial))
	{
		for (UProceduralMeshComponent* GenerateTerrain : GenerateTerrainArray)
		{
			if (GenerateTerrain)
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
			GenerateTerrain->ClearAllMeshSections();
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
		if (!GenerateTerrain)
			return;

		// 빈 UProceduralMeshComponent가 있으면 하나의 Section만 생성 및 NavOctree  갱신
		GenerateTerrain->CreateMeshSection(0, *SectionData.Vertices, *SectionData.Triangles,
			*SectionData.Normals, *SectionData.UVs, TArray<FColor>(),* SectionData.Tangents, true);
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
		// 해당 섹션이 있다면 메쉬 섹션 삭제 및 큐에 컴포넌트 반납
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


