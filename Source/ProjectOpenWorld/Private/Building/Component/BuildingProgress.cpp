#include "Building/Component/BuildingProgress.h"

UBuildingProgress::UBuildingProgress()
{
	PrimaryComponentTick.bCanEverTick = true;
	//Script/Engine.Material'/Game/Building/Mesh/Material/M_BuildingMaking.M_BuildingMaking'
	ConstructorHelpers::FObjectFinder< UMaterial> MakingMat(TEXT("/Game/Building/Mesh/Material/M_BuildingMaking.M_BuildingMaking"));
	if (MakingMat.Succeeded())
	{
		buildingMakingMat = MakingMat.Object;
	}
}

void UBuildingProgress::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(false);
	buildingMeshComponent = GetOwner()->GetComponentByClass<UStaticMeshComponent>();
	if (buildingMesh)
		SetbuildingMesh(buildingMesh.Get());
}

#if WITH_EDITOR
void UBuildingProgress::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (GetOwner())
	{
		buildingMeshComponent = GetOwner()->GetComponentByClass<UStaticMeshComponent>();
	}
	if (buildingMesh)
		SetbuildingMesh(buildingMesh.Get());

	int nSize = buildingMaking.Num();
	for (int i = 0; i < nSize; i++)
	{
		if (buildingMaking[i])
			buildingMaking[i]->SetScalarParameterValue(TEXT("FillPercent"), curentPercent);
	}
}
#endif

void UBuildingProgress::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (buildingMeshComponent)
	{
		curentPercent += DeltaTime / buildingTime;
		curentPercent = FMath::Clamp(curentPercent, 0.f, 1.f);
		int nSize = buildingMaking.Num();
		for (int i = 0; i < nSize; i++)
		{
			if (buildingMaking[i])
				buildingMaking[i]->SetScalarParameterValue(TEXT("FillPercent"), curentPercent);
		}
		if (curentPercent >= 1.0f)
			EndBuilding();
	}
}


void UBuildingProgress::SetbuildingMesh(UStaticMesh* NewMesh)
{
	buildingMesh = NewMesh;
	if (buildingMeshComponent && buildingMesh)
	{
		buildingMeshComponent->SetStaticMesh(buildingMesh.Get());
		if (buildingMakingMat)
		{
			int nSize = buildingMeshComponent->GetMaterials().Num();
			buildingMaking.Empty(nSize);


			for (int i = 0; i < nSize ; i++)
			{
				if (UMaterialInstanceDynamic* Making = buildingMeshComponent->CreateDynamicMaterialInstance(i, buildingMakingMat.Get()))
				{
					Making->SetScalarParameterValue(TEXT("MeshHeight"), buildingMeshComponent->GetStaticMesh()->GetBoundingBox().GetSize().Z);
					Making->SetScalarParameterValue(TEXT("MeshWorldHeight"),
						GetOwner()->GetActorLocation().Z - buildingMeshComponent->GetStaticMesh()->GetBoundingBox().Min.Z);
					Making->SetScalarParameterValue(TEXT("FillPercent"), curentPercent);
					buildingMeshComponent->SetMaterial(i, Making);
					buildingMaking.Add(Making);
				}
			}
		}
	}
}

void UBuildingProgress::StopBuilding()
{
	SetComponentTickEnabled(false);
}

void UBuildingProgress::ResumeBuilding()
{
	SetComponentTickEnabled(true);
}

void UBuildingProgress::StartBuilding()
{
	SetComponentTickEnabled(true);
	curentPercent = 0.0f;
	int nSize = buildingMaking.Num();
	for (int i = 0; i < nSize; i++)
	{
		UMaterialInstanceDynamic* Making = buildingMaking[i].Get();
		buildingMeshComponent->SetMaterial(i, Making);
		buildingMaking[i]->SetScalarParameterValue(TEXT("FillPercent"), curentPercent);
	}
}

void UBuildingProgress::EndBuilding()
{
	int nSize = buildingMaking.Num();
	for (int i = 0; i < nSize; i++)
	{
		buildingMeshComponent->SetMaterial(i, nullptr);
	}
	if (buildingMeshComponent && buildingMesh)
		buildingMeshComponent->SetStaticMesh(buildingMesh.Get());
	SetComponentTickEnabled(false);
}


