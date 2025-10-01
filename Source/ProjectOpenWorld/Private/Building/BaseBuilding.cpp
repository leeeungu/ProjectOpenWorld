#include "Building/BaseBuilding.h"
#include "Components/StaticMeshComponent.h"

ABaseBuilding::ABaseBuilding()
{
	PrimaryActorTick.bCanEverTick = true;
	buildingMeshComponent = CreateDefaultSubobject< UStaticMeshComponent>("BuildingMesh");
	if (buildingMeshComponent)
		buildingMeshComponent->SetupAttachment(RootComponent);
	//Script/Engine.Material'/Game/Building/Material/M_BuildingMaking.M_BuildingMaking'
	ConstructorHelpers::FObjectFinder< UMaterial> MakingMat(TEXT("/Game/Building/Material/M_BuildingMaking.M_BuildingMaking"));
	if (MakingMat.Succeeded())
	{
		buildingMakingMat = MakingMat.Object;
	}
}

void ABaseBuilding::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseBuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (buildingMaking && buildingMeshComponent)
	{
		curentPercent += DeltaTime / buildingTime;
		curentPercent = FMath::Clamp(curentPercent, 0.f, 1.f);
		buildingMaking->SetScalarParameterValue(TEXT("FillPercent"), curentPercent);
		if (curentPercent >= 1.0f)
		{
			buildingMeshComponent->SetMaterial(0, nullptr);
			if (buildingMeshComponent && buildingMesh)
			{
				buildingMeshComponent->SetStaticMesh(buildingMesh.Get());
			}
			SetActorTickEnabled(false);
		}
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
	}
	if (buildingMakingMat && buildingMeshComponent && buildingMeshComponent->GetStaticMesh())
	{
		if (buildingMaking = buildingMeshComponent->CreateDynamicMaterialInstance(0, buildingMakingMat.Get()))
		{
			buildingMaking->SetScalarParameterValue(TEXT("MeshHeight"), buildingMeshComponent->GetStaticMesh()->GetBoundingBox().GetSize().Z);
			buildingMaking->SetScalarParameterValue(TEXT("MeshWorldHeight"),
				GetActorLocation().Z - buildingMeshComponent->GetStaticMesh()->GetBoundingBox().Min.Z);
		}
	}
}

