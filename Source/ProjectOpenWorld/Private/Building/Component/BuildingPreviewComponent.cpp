#include "Building/Component/BuildingPreviewComponent.h"
#include "Kismet/GameplayStatics.h"

UBuildingPreviewComponent::UBuildingPreviewComponent(const FObjectInitializer& ObjectInitializer) : 
	UStaticMeshComponent(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetCollisionProfileName(TEXT("NoCollision"));

	static ConstructorHelpers::FObjectFinder< UMaterial> PreviewMat(TEXT("/Game/Building/Mesh/Material/M_BuildingPreview.M_BuildingPreview"));
	if (PreviewMat.Succeeded())
	{
		buildingPreviewMat = PreviewMat.Object;
	}
	SetActive(false);
	SetVisibility(false);
	SetHiddenInGame(true);
	SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	SetGenerateOverlapEvents(false);
	SetSimulatePhysics(false);
}

void UBuildingPreviewComponent::BeginPlay()
{
	Super::BeginPlay();
	buildingPreview = UMaterialInstanceDynamic::Create(buildingPreviewMat.Get(), this);

	if (TargetBuildingMesh)	
		SetBuildingMsh(TargetBuildingMesh.Get());
}

void UBuildingPreviewComponent::StartPreView()
{
	SetHiddenInGame(false);
	SetVisibility(true);
	UE_LOG(LogTemp, Warning, TEXT("%s"),*GetComponentLocation().ToString());

}

void UBuildingPreviewComponent::EndPreView()
{
	SetHiddenInGame(true);
	SetVisibility(false);
}

void UBuildingPreviewComponent::SetBuildingMsh(UStaticMesh* NewMesh)
{
	if (!NewMesh)
		return;

	TargetBuildingMesh = NewMesh;
	SetStaticMesh(TargetBuildingMesh.Get());

	int nSize = GetMaterials().Num();
	for (int i = 0; i < nSize; i++)
	{
		SetMaterial(i, buildingPreview.Get());
	}
}

void UBuildingPreviewComponent::SetBuildable(bool bValue)
{
	if (buildingPreview)
	{
		bBuildable = bValue;
		buildingPreview->SetScalarParameterValue(TEXT("Buildable"), !bValue);
	}
}

void UBuildingPreviewComponent::OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{
	Super::OnUpdateTransform(UpdateTransformFlags, Teleport);
	if (bHiddenInGame)
		return;

	// Preview Mesh가 이동 되고 그 위치에서의 건설 가능 채크
	bool bResult = true;

	TArray<TEnumAsByte<EObjectTypeQuery> > buildCheckObjectTypes{};
	buildCheckObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	buildCheckObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	buildCheckObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));
	buildCheckObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<FHitResult> ArrayPenetratingResult{};
	if (UKismetSystemLibrary::BoxTraceMultiForObjects(GetWorld(),
		GetComponentLocation()+ FVector(0,0, TargetBuildingMesh->GetBoundingBox().GetSize().Z * 0.5f),
		GetComponentLocation()+ FVector(0,0, TargetBuildingMesh->GetBoundingBox().GetSize().Z * 0.5f),
		TargetBuildingMesh->GetBoundingBox().GetSize() *0.5f, GetComponentRotation(),
		buildCheckObjectTypes, true, {}, EDrawDebugTrace::Type::ForOneFrame, ArrayPenetratingResult, true, FLinearColor::Black))
	{
		for (const FHitResult& PenetratingResult : ArrayPenetratingResult)
		{
			if (PenetratingResult.bStartPenetrating &&  PenetratingResult.PenetrationDepth > 5.0f && PenetratingResult.GetComponent() != this)
			{
				bResult = false;
				break;
			}
		}
	}
	SetBuildable(bResult);
}