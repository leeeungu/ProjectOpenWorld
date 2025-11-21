#include "Building/Actor/BuildingPreviewActor.h"
#include "Engine/StaticMeshSocket.h"
#include "AssetToolsModule.h"
#include "Subsystems/EditorAssetSubsystem.h"

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

	//if (BuildingMesh && BuildingMesh->GetStaticMesh())
	//{
	//	UStaticMesh* Mesh = BuildingMesh->GetStaticMesh();
	//	UE_LOG(LogTemp, Warning, TEXT("%d"), BuildingMesh->GetStaticMesh()->Sockets.Num());
	//	Mesh->Modify(); // ⭐ 반드시 먼저 호출 (Undo/Redo + 에셋 수정 시작)

	//	MeshSocket = Mesh->FindSocket(TEXT("TestSocket"));
	//	if (!MeshSocket)
	//	{
	//		MeshSocket = NewObject<UStaticMeshSocket>(Mesh); // Outer = Mesh
	//		MeshSocket->SocketName = TEXT("TestSocket");
	//		MeshSocket->RelativeLocation = FVector(0, 0, 0);
	//		MeshSocket->RelativeRotation = FRotator(0, 0, 0);
	//		MeshSocket->RelativeScale = FVector(1.f);
	//		Mesh->AddSocket(MeshSocket);
	//		BuildingMesh->GetStaticMesh()->Sockets.Empty();
	//		Mesh->PostEditChange();
	//		//if (GEditor)
	//		//{
	//		//	UEditorAssetSubsystem* Asset = GEditor->GetEditorSubsystem<UEditorAssetSubsystem>();
	//		//	if (Asset)
	//		//	{
	//		//		Asset->SaveLoadedAsset(Mesh);
	//		//	}
	//		//}
	//		Mesh->MarkPackageDirty();
	//	}
	//}
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
