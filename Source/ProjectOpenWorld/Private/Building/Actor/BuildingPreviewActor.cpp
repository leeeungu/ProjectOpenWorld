#include "Building/Actor/BuildingPreviewActor.h"

ABuildingPreviewActor::ABuildingPreviewActor()
{
	PrimaryActorTick.bCanEverTick = false;
	// //Script/Engine.Material'/Game/Building/Mesh/Material/M_BuildingPreview.M_BuildingPreview'
	static ConstructorHelpers::FObjectFinder< UMaterial> PreviewMat(TEXT("/Game/Building/Mesh/Material/M_BuildingPreview.M_BuildingPreview"));
	if (PreviewMat.Succeeded())
	{
		buildingPreviewMat = PreviewMat.Object;
	}
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	BuildingMesh->SetupAttachment(RootComponent);
}

void ABuildingPreviewActor::BeginPlay()
{
	Super::BeginPlay();
	buildingPreview = UMaterialInstanceDynamic::Create(buildingPreviewMat.Get(), this);
}

void ABuildingPreviewActor::SetBuildingMsh(UStaticMesh* NewMesh)
{
	if (!NewMesh)
		return;

	TargetBuildingMesh = NewMesh;
	BuildingMesh->SetStaticMesh(TargetBuildingMesh.Get());

	int nSize = BuildingMesh->GetMaterials().Num();
	for (int i = 0; i < nSize; i++)
	{
		BuildingMesh->SetMaterial(i, buildingPreview.Get());
	}
	//meshSize = BuildingMesh->GetBoundingBox().GetSize();
	//FTransform Relative = buildingPreviewActor->GetStaticMeshComponent()->GetSocketTransform(TEXT("BuildingCenter"), ERelativeTransformSpace::RTS_Component);
	//meshCenter = Relative.GetLocation();
	////meshSize.Z = abs(meshCenter.Z * 0.25f);
	////buildingPreviewActor->SetActorRelativeLocation(meshCenter * -1);
	//UE_LOG(LogTemp, Warning, TEXT("meshCenter : %s"), *meshCenter.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("MeshSize : %s"), *meshSize.ToString());
	////buildingPreviewActor->GetStaticMeshComponent()->SetRelativeLocation({ 0,0,meshSize.Z });
	//UpdatePreviewMat();
}

void ABuildingPreviewActor::SetBuildable(bool bValue)
{
	if (buildingPreview)
	{
		buildingPreview->SetScalarParameterValue(TEXT("Buildable"), !bValue);
	}
}
