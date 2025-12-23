#include "Building/Component/BuildingProgress.h"
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
	//StopBuilding();
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
	}
}

void UBuildingProgress::StopBuilding(TScriptInterface<IArchitectureInterface> OtherInstigator)
{
	if (curentPercent >= 1.0f)
	{
		buildSpeed = 0;
		return;
	}
	if (OtherInstigator && OtherInstigator.GetObject())
	{
		if (InstigatorList.Find(OtherInstigator.GetObject()))
		{
			float Speed = IArchitectureInterface::Execute_GetArchitectSpeed(OtherInstigator.GetObject());
			buildSpeed -= Speed;
			SetComponentTickEnabled(false);
			isBuilding = false;
			IArchitectureInterface::Execute_StopArchitect(OtherInstigator.GetObject(), Cast<ABaseBuilding >(GetOwner()));
			InstigatorList.Remove(OtherInstigator.GetObject());
		}
	}
	if (InstigatorList.IsEmpty())
	{
		DeActiveBuildingNav();
	}
}
void UBuildingProgress::StopAll()
{
	for (TWeakObjectPtr<UObject>& Other : InstigatorList)
	{
		if (Other.IsValid())
		{
			IArchitectureInterface::Execute_StopArchitect(Other.Get(), Cast<ABaseBuilding >(GetOwner()));
		}
	}
	DeActiveBuildingNav();
	InstigatorList.Empty(0);
	buildSpeed = 0;
	SetComponentTickEnabled(false);
	isBuilding = false;
}

void UBuildingProgress::StartBuilding(TScriptInterface<IArchitectureInterface> OtherInstigator)
{
	if (curentPercent >= 1.0f)
	{
		buildSpeed = 0;
		return;
	}
	if (OtherInstigator && OtherInstigator.GetObject())
	{
		if (InstigatorList.Find(OtherInstigator.GetObject()))
		{
			//UE_LOG(LogTemp, Log, TEXT("UBuildingProgress::StartBuilding Already has"));
			return;
		}
		InstigatorList.Add(OtherInstigator.GetObject());
		float Speed = IArchitectureInterface::Execute_GetArchitectSpeed(OtherInstigator.GetObject());// OtherInstigator->GetArchitectSpeed();
		buildSpeed += Speed;
		if (!isBuilding)
		{
			
			isBuilding = true;
			SetComponentTickEnabled(true);
		}
		IArchitectureInterface::Execute_StartArchitect(OtherInstigator.GetObject(), Cast<ABaseBuilding >(GetOwner()));
	}
	else
		UE_LOG(LogTemp, Error, TEXT("UBuildingProgress::StartBuilding NonInterface"));
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
	ActiveBuildingNav();

	SetComponentTickEnabled(false);
	curentPercent = 1.0f; 

	for (TWeakObjectPtr<UObject>& Other : InstigatorList)
	{
		if (Other.IsValid())
		{
			IArchitectureInterface::Execute_EndArchitect(Other.Get(), Cast<ABaseBuilding >(GetOwner()));
		}
	}
	InstigatorList.Empty(0);

	if (onBuildingEnd.IsBound())
	{
		//UE_LOG(LogTemp, Warning, TEXT("BuildingProgress :: BreadCast %d"), onBuildingEnd.GetAllObjects().Num());
		//for (UObject* Obj : onBuildingEnd.GetAllObjects())
		//{
		//	//UE_LOG(LogTemp, Warning, TEXT("%s"), *Obj->GetFName().ToString());
		//}
		onBuildingEnd.Broadcast();
	}

	/*if(ABaseBuilding* OwnerActor = Cast<ABaseBuilding>(GetOwner()))
	{
		OwnerActor->UpdateModifier();
	}*/
}

bool UBuildingProgress::IsBuildingEnd() const
{
	return curentPercent >= 1.0f || FMath::IsNearlyEqual(curentPercent,1.0f);
}

void UBuildingProgress::ActiveBuildingNav()
{
	if (ABaseBuilding* OwnerActor = Cast<ABaseBuilding>(GetOwner()))
	{
		OwnerActor->UpdateModifier();
	}
}

void UBuildingProgress::DeActiveBuildingNav()
{
	if (!InstigatorList.IsEmpty())
		return;
	if (ABaseBuilding* OwnerActor = Cast<ABaseBuilding>(GetOwner()))
	{
		OwnerActor->NoCollision();
	}
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


