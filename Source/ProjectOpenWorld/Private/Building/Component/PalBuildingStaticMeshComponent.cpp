#include "Building/Component/PalBuildingStaticMeshComponent.h"
#include "Building/Actor/BuildingActor.h"
#include "Building/Subsystem/BuildingDataSubsystem.h"
#include "UObject/ObjectSaveContext.h"

UPalBuildingStaticMeshComponent::UPalBuildingStaticMeshComponent(const FObjectInitializer& ObjectInitializer ) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	//Script/Engine.Material'/Game/Building/Mesh/Material/M_BuildingMaking.M_BuildingMaking'
	ConstructorHelpers::FObjectFinder< UMaterial> MakingMat(TEXT("/Game/Building/Mesh/Material/M_BuildingMaking.M_BuildingMaking"));
	if (MakingMat.Succeeded())
	{
		buildingMakingMat = MakingMat.Object;
	}
}

void UPalBuildingStaticMeshComponent::BeginPlay()
{
	Super::BeginPlay();
	//StopBuilding();
	ABuildingActor* Building = Cast<ABuildingActor>(GetOwner());
	if (!Building)
		return;
	buildingTime = UBuildingDataSubsystem::GetPalBuildObjectRequiredBuildWorkAmountByName(Building->GetBuildingID());
	buildSpeed = 0.0f;
	curentPercent = 0.0f;
	isBuilding = false;
	SetBuildingPercent(curentPercent);
}

#if WITH_EDITOR
void UPalBuildingStaticMeshComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (GetOwner())
	{
		ABuildingActor* Building = Cast<ABuildingActor>(GetOwner());
		if (Building)
		{
			buildingTime = UBuildingDataSubsystem::GetPalBuildObjectRequiredBuildWorkAmountByName(Building->GetBuildingID());
		}
	}
	SetBuildingPercent(0.3);
}
#endif

void UPalBuildingStaticMeshComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
		curentPercent += (DeltaTime * buildSpeed) / buildingTime;
		curentPercent = FMath::Clamp(curentPercent, 0.f, 1.f);
		SetBuildingPercent(curentPercent);
		if (FMath::IsNearlyEqual(curentPercent, 1.0f))
		{
			EndBuilding();
		}
}

//void UPalBuildingStaticMeshComponent::SetbuildingMesh(UStaticMesh* NewMesh)
//{
//	//buildingMesh = NewMesh;
//	//ABuildingActor* Building = Cast<ABuildingActor>(GetOwner());
//	//if (!Building)
//	//	return;
//	//buildingMesh = Building->GetBuildingMeshComponent()->GetStaticMesh();
//
//}

void UPalBuildingStaticMeshComponent::StopBuilding(TScriptInterface<IArchitectureInterface> OtherInstigator)
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
void UPalBuildingStaticMeshComponent::StopAll()
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

void UPalBuildingStaticMeshComponent::StartBuilding(TScriptInterface<IArchitectureInterface> OtherInstigator)
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
			//UE_LOG(LogTemp, Log, TEXT("UPalBuildingStaticMeshComponent::StartBuilding Already has"));
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
		UE_LOG(LogTemp, Error, TEXT("UPalBuildingStaticMeshComponent::StartBuilding NonInterface"));
}

void UPalBuildingStaticMeshComponent::EndBuilding()
{
	isBuilding = false;
	int nSize = buildingMaking.Num();
	buildSpeed = 0;
	for (int i = 0; i < nSize; i++)
	{
		SetMaterial(i, nullptr);
	}
	//if (buildingMeshComponent && buildingMesh)
	//	buildingMeshComponent->SetStaticMesh(buildingMesh.Get());
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

	//if (onBuildingEnd.IsBound())
	//{
	//	//UE_LOG(LogTemp, Warning, TEXT("BuildingProgress :: BreadCast %d"), onBuildingEnd.GetAllObjects().Num());
	//	//for (UObject* Obj : onBuildingEnd.GetAllObjects())
	//	//{
	//	//	//UE_LOG(LogTemp, Warning, TEXT("%s"), *Obj->GetFName().ToString());
	//	//}
	//	onBuildingEnd.Broadcast();
	//}

	/*if(ABaseBuilding* OwnerActor = Cast<ABaseBuilding>(GetOwner()))
	{
		OwnerActor->UpdateModifier();
	}*/
}

bool UPalBuildingStaticMeshComponent::SetStaticMesh(UStaticMesh* NewMesh)
{
	bool Result = Super::SetStaticMesh(NewMesh);
	if (NewMesh)
	{
		//curentPercent = 0;
		//SetMobility(EComponentMobility::Movable);
		//SetMobility(EComponentMobility::Static);
		//if (buildingMakingMat)
		//{
		//	int nSize = GetMaterials().Num();
		//	buildingMaking.Empty(nSize);


		//	for (int i = 0; i < nSize; i++)
		//	{
		//		if (UMaterialInstanceDynamic* Making = CreateDynamicMaterialInstance(i, buildingMakingMat.Get()))
		//		{
		//			Making->SetScalarParameterValue(TEXT("MeshHeight"), NewMesh->GetBoundingBox().GetSize().Z);
		//			float MeshWorldHeight = GetOwner()->GetActorLocation().Z;
		//			// -buildingMeshComponent->GetStaticMesh()->GetBoundingBox().GetSize().Z
		//			//	- GetOwner()->GetActorLocation().Z + buildingMeshComponent->GetStaticMesh()->GetBounds().BoxExtent.Y;
		//			Making->SetScalarParameterValue(TEXT("MeshWorldHeight"),
		//				MeshWorldHeight);

		//			//UE_LOG(LogTemp, Warning, TEXT("MeshHeight : %s"), *buildingMeshComponent->GetStaticMesh()->GetBounds().ToString());
		//			SetMaterial(i, Making);
		//			buildingMaking.Add(Making);
		//		}
		//	}
		//}
	}
	return Result;
}

bool UPalBuildingStaticMeshComponent::IsBuildingEnd() const
{
	return curentPercent >= 1.0f || FMath::IsNearlyEqual(curentPercent, 1.0f);
}

void UPalBuildingStaticMeshComponent::ActiveBuildingNav()
{
	if (ABaseBuilding* OwnerActor = Cast<ABaseBuilding>(GetOwner()))
	{
		OwnerActor->UpdateModifier();
	}
}

void UPalBuildingStaticMeshComponent::DeActiveBuildingNav()
{
	if (!InstigatorList.IsEmpty())
		return;
	if (ABaseBuilding* OwnerActor = Cast<ABaseBuilding>(GetOwner()))
	{
		OwnerActor->NoCollision();
	}
}

void UPalBuildingStaticMeshComponent::SetBuildingPercent(float Value)
{
	int nSize = buildingMaking.Num();
	curentPercent = Value;
	for (int i = 0; i < nSize; i++)
	{
		if (buildingMaking[i])
		{
			UMaterialInstanceDynamic* Making = buildingMaking[i].Get();
			SetMaterial(i, Making);
			buildingMaking[i]->SetScalarParameterValue(TEXT("FillPercent"), Value);
		}
	}
}


