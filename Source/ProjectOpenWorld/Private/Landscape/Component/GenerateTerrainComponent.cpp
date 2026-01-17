#include "Landscape/Component/GenerateTerrainComponent.h"
#include "ProceduralMeshComponent.h"
#include "NavigationSystem.h"

UGenerateTerrainComponent::UGenerateTerrainComponent() : Super()
{
	PrimaryComponentTick.bCanEverTick = false;
	TargetTerrain = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("TargetTerrain"));
	GenerateTerrain = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GenerateTerrain"));
	//GenerateTerrain->ClearAllMeshSections();
	//TargetTerrain->ClearAllMeshSections();
}

void UGenerateTerrainComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = PropertyChangedEvent.GetPropertyName();
	if(PropertyName == GET_MEMBER_NAME_CHECKED(UGenerateTerrainComponent, TerrainMaterial))
	{
		if (TargetTerrain && TerrainMaterial)
		{
			int32 SectionCount = TargetTerrain->GetNumSections();
			for (int32 i = 0; i < SectionCount; ++i)
			{
				TargetTerrain->SetMaterial(i, TerrainMaterial);
			}
			SectionCount = GenerateTerrain->GetNumSections();
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


void UGenerateTerrainComponent::StartGenerateWorld()
{
	if (TargetTerrain != GenerateTerrain)
	{
		if (TargetTerrain)
		{
			TargetTerrain->SetVisibility(false);
			TargetTerrain->SetActive(false);
			TargetTerrain->SetComponentTickEnabled(false);
		}
		UProceduralMeshComponent* Temp = TargetTerrain;
		TargetTerrain = GenerateTerrain;
		//UE_LOG(LogTemp, Warning, TEXT("AWorldGenerator Set Current Mesh %s %s"), *TargetTerrain->GetName(), *Temp->GetName());
		GenerateTerrain = Temp;
		if (TargetTerrain)
		{
			TargetTerrain->SetVisibility(true);
			TargetTerrain->SetActive(true);
			TargetTerrain->SetComponentTickEnabled(true);
			UNavigationSystemV1::UpdateComponentInNavOctree(*TargetTerrain);
		}
	}
}

void UGenerateTerrainComponent::UpdateGenerateWorld(int SectionIndex, const TArray<FVector>& Vertices, const TArray<FVector2D>& UVs, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FProcMeshTangent>& Tangents)
{
	//UE_LOG(LogTemp, Warning, TEXT("UGenerateTerrainComponent Update Mesh %d Vertices:%d"), SectionIndex, Vertices.Num());
	if (!GenerateTerrain)
		return;

	if (GenerateTerrain->GetProcMeshSection(SectionIndex) && Vertices.Num()
		== GenerateTerrain->GetProcMeshSection(SectionIndex)->ProcVertexBuffer.Num())
	{
		GenerateTerrain->UpdateMeshSection(SectionIndex, Vertices, Normals, UVs, TArray<FColor>(), Tangents);
	}
	else if (!GenerateTerrain->GetProcMeshSection(SectionIndex) ||
		Vertices.Num() != GenerateTerrain->GetProcMeshSection(SectionIndex)->ProcVertexBuffer.Num())
	{
		//UE_LOG(LogTemp, Warning, TEXT("UGenerateTerrainComponent Create Mesh Section %d Vertices:%d"), SectionIndex, Vertices.Num());
		GenerateTerrain->CreateMeshSection(SectionIndex, Vertices, Triangles, Normals, UVs, TArray<FColor>(), Tangents, true);
	}
	if (TerrainMaterial)
	{
		GenerateTerrain->SetMaterial(SectionIndex, TerrainMaterial);
	}
}

void UGenerateTerrainComponent::Initialize(USceneComponent* ParentComponent)
{
	//if (SectionComponent)
	//{
	//	SectionComponent->OnUpdateSection.AddUniqueDynamic(this, &UGenerateTerrainComponent::UpdateMesh);
	//	SectionComponent->OnGenerateFinished.AddUniqueDynamic(this, &UGenerateTerrainComponent::SetCurrentMesh);
	//}
	if (ParentComponent)
	{
		TargetTerrain->SetupAttachment(ParentComponent);
		GenerateTerrain->SetupAttachment(ParentComponent);
	}
}

//void UGenerateTerrainComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//}

