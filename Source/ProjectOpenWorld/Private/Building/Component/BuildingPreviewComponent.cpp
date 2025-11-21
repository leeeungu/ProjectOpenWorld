#include "Building/Component/BuildingPreviewComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/DataTableFunctionLibrary.h"

UBuildingPreviewComponent::UBuildingPreviewComponent(const FObjectInitializer& ObjectInitializer) : 
	UStaticMeshComponent(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
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
	EndPreView();
	//Script/Engine.DataTable'/Game/Building/Data/DT_SnapData.DT_SnapData'
	static ConstructorHelpers::FObjectFinder< UDataTable> MeshData(TEXT("/Game/Building/Data/DT_SnapData.DT_SnapData"));
	UDataTable* pDT{};
	if (MeshData.Succeeded())
	{
		pDT = MeshData.Object;
	}
	if (pDT)
	{
		pDT->GetAllRows< FSnapRule>(TEXT(""), ArrData);
		SnapParentSet.Reserve(ArrData.Num());
		SnapParentArray.Reserve(ArrData.Num());
	}
}

void UBuildingPreviewComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	UStaticMeshComponent::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!HittedResult.bBlockingHit || !HittedResult.GetActor())
	{
		SetComponentTickEnabled(false);
		return;
	}

	bSnapped = false;
	if (!SnapParentArray.IsEmpty() && ParentMesh)
	{
		FTransform ParentWorld = ParentMesh->GetComponentTransform();
		FTransform ComponentTrans = GetComponentTransform();
		for (const auto& sanpRule : SnapParentArray)
		{
			const FTransform& ParentSocketWorld = sanpRule->ParentAnchorLocal * ParentMesh->GetComponentTransform();
			const FVector ParentSocketWorldPos = ParentSocketWorld.GetLocation();
			if ( FVector::DistSquared(HitLocation, ParentSocketWorldPos) <= 80.0f * 80.0f)
			{
				bSnapped = true;
				const FTransform& ChildSocketLocal = sanpRule->ChildAnchorLocal;

				// 목적: ChildWorld * ChildSocketLocal = ParentSocketWorld

				// 회전 계산
				const FQuat ParentSocketWorldRot = ParentSocketWorld.GetRotation();
				const FQuat ChildSocketLocalRot = ChildSocketLocal.GetRotation();
				const FQuat ChildWorldRot = ParentSocketWorldRot * ChildSocketLocalRot.Inverse();

				// 위치 계산
				const FVector ChildSocketLocalPos = ChildSocketLocal.GetLocation();
				const FVector ChildSocketWorldOffset = ChildWorldRot.RotateVector(ChildSocketLocalPos);
				const FVector ChildWorldPos = ParentSocketWorldPos - ChildSocketWorldOffset;

				ComponentTrans.SetLocation(ChildWorldPos);
				ComponentTrans.SetRotation(ChildWorldRot);
				SetWorldTransform(ComponentTrans);
				return;
			}
		}
	}
	SetWorldLocation(HittedResult.ImpactPoint);
}

void UBuildingPreviewComponent::BeginPlay()
{
	Super::BeginPlay();
	buildingPreview = UMaterialInstanceDynamic::Create(buildingPreviewMat.Get(), this);
	DetachFromParent();
	SetWorldTransform(FTransform::Identity);
	if (TargetBuildingMesh)	
		SetBuildingMsh(TargetBuildingMesh.Get());
}

void UBuildingPreviewComponent::StartPreView()
{
	SetHiddenInGame(false);
	SetVisibility(true);
}

void UBuildingPreviewComponent::EndPreView()
{
	SetHiddenInGame(true);
	SetVisibility(false);
	ParentMesh = nullptr;
	SnapParentSet.Empty();
	SnapParentArray.Empty();
	SetComponentTickEnabled(false);
}

void UBuildingPreviewComponent::SetParentMesh(FHitResult& Hit, UStaticMeshComponent* NewMesh)
{
	HittedResult = Hit;
	ParentMesh = NewMesh;
	HitLocation = HittedResult.ImpactPoint;
	if (!HittedResult.bBlockingHit || !HittedResult.GetActor())
	{
		SetComponentTickEnabled(false);
		return;
	}
	SetComponentTickEnabled(true);
	SnapParentArray.Empty();
	for (const auto& Data : SnapParentSet)
	{
		if(ParentMesh && Data->ParentMesh == ParentMesh->GetStaticMesh() && Data->ChildMesh == TargetBuildingMesh)
			SnapParentArray.Add(Data);
	}
}

void UBuildingPreviewComponent::SetBuildingMsh(UStaticMesh* NewMesh)
{
	if (!NewMesh)
		return;

	TargetBuildingMesh = NewMesh;
	SetStaticMesh(TargetBuildingMesh.Get());
	SnapParentSet.Empty();
	for (const auto& Data : ArrData)
	{
		if (Data->ChildMesh == TargetBuildingMesh)
		{
			SnapParentSet.Add(Data);
		}
	}

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