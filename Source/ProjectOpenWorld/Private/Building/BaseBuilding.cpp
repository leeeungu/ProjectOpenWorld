#include "Building/BaseBuilding.h"
#include "Components/StaticMeshComponent.h"

ABaseBuilding::ABaseBuilding()
{
	PrimaryActorTick.bCanEverTick = true;
	buildingMeshComponent = CreateDefaultSubobject< UStaticMeshComponent>("BuildingMesh");
	if (buildingMeshComponent)
		buildingMeshComponent->SetupAttachment(RootComponent);
	//Script/Engine.Material'/Game/Building/Mesh/Material/M_BuildingMaking.M_BuildingMaking'
	ConstructorHelpers::FObjectFinder< UMaterial> MakingMat(TEXT("/Game/Building/Mesh/Material/M_BuildingMaking.M_BuildingMaking"));
	if (MakingMat.Succeeded())
	{
		buildingMakingMat = MakingMat.Object;
	}
	//SetReplicates(true);
	//bNetLoadOnClient = true;
	//SetReplicateMovement(true);
}

void ABaseBuilding::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);
}

void ABaseBuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (buildingMeshComponent)
	{
		curentPercent += DeltaTime / buildingTime;
		curentPercent = FMath::Clamp(curentPercent, 0.f, 1.f);
		int nSize = buildingMaking.Num();
		for (int i = 0; i < nSize; i++)
		{
			buildingMaking[i]->SetScalarParameterValue(TEXT("FillPercent"), curentPercent);
		}
		if (curentPercent >= 1.0f)
			EndBuilding();
	}
}

void ABaseBuilding::OnConstruction(const FTransform& Transform)
{
	AActor::OnConstruction(Transform);
	if (buildingMeshComponent && buildingMesh)
	{
		buildingMeshComponent->SetStaticMesh(buildingMesh.Get());
	}
}

void ABaseBuilding::SetbuildingMesh(UStaticMesh* NewMesh)
{
	buildingMesh = NewMesh;
	if (buildingMeshComponent && buildingMesh)
	{
		buildingMeshComponent->SetStaticMesh(buildingMesh.Get());
		if (buildingMakingMat)
		{
			int nSize = buildingMeshComponent->GetMaterials().Num();
			buildingMaking.Empty(nSize);
			for (int i = 0; i < nSize; i++)
			{
				if (UMaterialInstanceDynamic* Making = buildingMeshComponent->CreateDynamicMaterialInstance(i, buildingMakingMat.Get()))
				{
					Making->SetScalarParameterValue(TEXT("MeshHeight"), buildingMeshComponent->GetStaticMesh()->GetBoundingBox().GetSize().Z);
					Making->SetScalarParameterValue(TEXT("MeshWorldHeight"),
						GetActorLocation().Z - buildingMeshComponent->GetStaticMesh()->GetBoundingBox().Min.Z);
					Making->SetScalarParameterValue(TEXT("FillPercent"), curentPercent);
					buildingMaking.Add(Making);
				}
			}
		}
	}
}


void ABaseBuilding::StopBuilding()
{
	SetActorTickEnabled(false);
}
void ABaseBuilding::StartBuilding()
{
	SetActorTickEnabled(true);
	curentPercent = 0.0f;
	int nSize = buildingMaking.Num();
	for (int i = 0; i < nSize; i++)
	{
		UMaterialInstanceDynamic* Making = buildingMaking[i].Get();
		buildingMeshComponent->SetMaterial(i, Making);
	}
}
void ABaseBuilding::EndBuilding()
{
	int nSize = buildingMaking.Num();
	for (int i = 0; i < nSize; i++)
	{
		buildingMeshComponent->SetMaterial(i, nullptr);
	}
	if (buildingMeshComponent && buildingMesh)
		buildingMeshComponent->SetStaticMesh(buildingMesh.Get());
	SetActorTickEnabled(false);
}
