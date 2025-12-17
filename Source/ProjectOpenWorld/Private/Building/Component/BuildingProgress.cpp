#include "Building/Component/BuildingProgress.h"
#include "NavigationSystem.h"
#include "Building/BaseBuilding.h"

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
	StopBuilding();
	buildingMeshComponent = GetOwner()->GetComponentByClass<UStaticMeshComponent>();
	if (!buildingMeshComponent)
		return;

	if (buildingMesh != buildingMeshComponent->GetStaticMesh() || !buildingMesh)
	{
		buildingMesh = buildingMeshComponent->GetStaticMesh();
	}
		SetbuildingMesh(buildingMesh.Get());
	buildSpeed = 0.0f;
	curentPercent = 0.0f;
	isBuilding = false;
	SetBuildingPercent(curentPercent);

}

#if WITH_EDITOR
void UBuildingProgress::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (GetOwner())
	{
		buildingMeshComponent = GetOwner()->GetComponentByClass<UStaticMeshComponent>();
	}
	if (buildingMeshComponent)
	{
		if (buildingMesh != buildingMeshComponent->GetStaticMesh() || !buildingMesh)
		{
			buildingMesh = buildingMeshComponent->GetStaticMesh();
		}
		//SetbuildingMesh(buildingMeshComponent->GetStaticMesh());
	}

	SetBuildingPercent(0.3);
}
#endif

void UBuildingProgress::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (buildingMeshComponent && isBuilding)
	{
		curentPercent += (DeltaTime * buildSpeed) / buildingTime;
		curentPercent = FMath::Clamp(curentPercent, 0.f, 1.f);
		SetBuildingPercent(curentPercent);
		if (FMath::IsNearlyEqual(curentPercent, 1.0f))
		{
			EndBuilding();
		}
	}
}

void UBuildingProgress::SetbuildingMesh(UStaticMesh* NewMesh)
{
	buildingMesh = NewMesh;
	if (buildingMeshComponent && buildingMesh)
	{
		curentPercent = 0;
		buildingMeshComponent->SetMobility(EComponentMobility::Movable);
		buildingMeshComponent->SetStaticMesh(buildingMesh.Get());
		buildingMeshComponent->SetMobility(EComponentMobility::Static);
		buildingMeshComponent->SetCanEverAffectNavigation(true);
		if (buildingMakingMat)
		{
			int nSize = buildingMeshComponent->GetMaterials().Num();
			buildingMaking.Empty(nSize);


			for (int i = 0; i < nSize ; i++)
			{
				if (UMaterialInstanceDynamic* Making = buildingMeshComponent->CreateDynamicMaterialInstance(i, buildingMakingMat.Get()))
				{
					Making->SetScalarParameterValue(TEXT("MeshHeight"), buildingMeshComponent->GetStaticMesh()->GetBoundingBox().GetSize().Z);
					float MeshWorldHeight = GetOwner()->GetActorLocation().Z;
					// -buildingMeshComponent->GetStaticMesh()->GetBoundingBox().GetSize().Z
					//	- GetOwner()->GetActorLocation().Z + buildingMeshComponent->GetStaticMesh()->GetBounds().BoxExtent.Y;
					Making->SetScalarParameterValue(TEXT("MeshWorldHeight"),
						MeshWorldHeight );

					//UE_LOG(LogTemp, Warning, TEXT("MeshHeight : %s"), *buildingMeshComponent->GetStaticMesh()->GetBounds().ToString());
					buildingMeshComponent->SetMaterial(i, Making);
					buildingMaking.Add(Making);
				}
			}
		}
		buildingMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

		ABaseBuilding* OwnerActor = Cast<ABaseBuilding>(GetOwner());
		if (OwnerActor)
		{
			OwnerActor->UpdateModifier();
		}
	}
}

void UBuildingProgress::StopBuilding()
{
	buildSpeed -= 1.0f;
	if (buildSpeed <= 0.f)
	{
		buildSpeed = 0.0f;
		SetComponentTickEnabled(false);
		isBuilding = false;
	}
}

void UBuildingProgress::ResumeBuilding()
{
	StartBuilding();
}

void UBuildingProgress::StartBuilding()
{
	
	if (curentPercent >= 1.0f)
	{
		buildSpeed = 0;
		return;
	}
	buildSpeed += 1.0f;
	isBuilding = true;
	SetComponentTickEnabled(true);
}

void UBuildingProgress::EndBuilding()
{
	isBuilding = false;
	int nSize = buildingMaking.Num();
	buildSpeed = 0;
	for (int i = 0; i < nSize; i++)
	{
		buildingMeshComponent->SetMaterial(i, nullptr);
	}
	if (buildingMeshComponent && buildingMesh)
		buildingMeshComponent->SetStaticMesh(buildingMesh.Get());
	SetComponentTickEnabled(false);
	curentPercent = 1.0f;
	if (onBuildingEnd.IsBound())
	{
		//UE_LOG(LogTemp, Warning, TEXT("BuildingProgress :: BreadCast %d"), onBuildingEnd.GetAllObjects().Num());
		//for (UObject* Obj : onBuildingEnd.GetAllObjects())
		//{
		//	//UE_LOG(LogTemp, Warning, TEXT("%s"), *Obj->GetFName().ToString());
		//}
		onBuildingEnd.Broadcast();
	}
	buildingMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	buildingMeshComponent->SetCanEverAffectNavigation(true);
	UNavigationSystemV1* Nav = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	Nav->AddDirtyArea(buildingMeshComponent->GetStaticMesh()->GetBounds().GetBox(), 0);
}

bool UBuildingProgress::IsBuildingEnd() const
{
	return curentPercent >= 1.0f || FMath::IsNearlyEqual(curentPercent,1.0f);
}

void UBuildingProgress::SetBuildingPercent(float Value)
{
	int nSize = buildingMaking.Num();
	curentPercent = Value;
	for (int i = 0; i < nSize; i++)
	{
		if (buildingMaking[i])
		{
			UMaterialInstanceDynamic* Making = buildingMaking[i].Get();
			buildingMeshComponent->SetMaterial(i, Making);
			buildingMaking[i]->SetScalarParameterValue(TEXT("FillPercent"), Value);
		}
	}
}


