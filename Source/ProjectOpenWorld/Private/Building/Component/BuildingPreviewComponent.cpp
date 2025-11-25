#include "Building/Component/BuildingPreviewComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMesh.h"

UBuildingPreviewComponent::UBuildingPreviewComponent(const FObjectInitializer& ObjectInitializer)
	: UStaticMeshComponent(ObjectInitializer)
{
	// 이동 자체는 Assist 컴포넌트가 담당하므로 Tick은 사실상 필요 없음
	PrimaryComponentTick.bCanEverTick = false;

	// 프리뷰는 충돌체로 동작하지 않음 (겹침 판정은 OnUpdateTransform에서 별도 Trace로 처리)
	SetCollisionProfileName(TEXT("NoCollision"));
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetGenerateOverlapEvents(false);
	SetSimulatePhysics(false);

	// 프리뷰용 머티리얼 로딩
	static ConstructorHelpers::FObjectFinder<UMaterial> PreviewMat(
		TEXT("/Game/Building/Mesh/Material/M_BuildingPreview.M_BuildingPreview"));
	if (PreviewMat.Succeeded())
	{
		buildingPreviewMat = PreviewMat.Object;
	}

	SetActive(false);
	SetVisibility(false);
	SetHiddenInGame(true);
}

void UBuildingPreviewComponent::BeginPlay()
{
	Super::BeginPlay();

	if (buildingPreviewMat)
	{
		buildingPreview = UMaterialInstanceDynamic::Create(buildingPreviewMat.Get(), this);
	}

	// 월드 상 부모와 분리, 기준은 월드 Identity
	DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	SetWorldTransform(FTransform::Identity);

	// 시작 시점에 설정된 메쉬가 있다면 적용
	if (TargetBuildingMesh)
	{
		SetBuildingMsh(TargetBuildingMesh.Get());
	}
}

void UBuildingPreviewComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// 이동은 Assist 컴포넌트가 담당하므로 여기서는 아무 것도 하지 않음
}

void UBuildingPreviewComponent::StartPreView()
{
	SetHiddenInGame(false);
	SetVisibility(true, true);
}

void UBuildingPreviewComponent::EndPreView()
{
	SetHiddenInGame(true);
	SetVisibility(false, true);
}

void UBuildingPreviewComponent::SetBuildingMsh(UStaticMesh* NewMesh)
{
	if (!NewMesh)
	{
		return;
	}

	TargetBuildingMesh = NewMesh;
	SetStaticMesh(TargetBuildingMesh.Get());

	BottomTrans = GetSocketTransform(TEXT("Bottom"), ERelativeTransformSpace::RTS_Component);
	// 모든 머티리얼 슬롯에 프리뷰용 동적 머티리얼 적용
	if (buildingPreview)
	{
		const int32 NumMats = GetMaterials().Num();
		for (int32 Index = 0; Index < NumMats; ++Index)
		{
			SetMaterial(Index, buildingPreview);
		}
	}
}

void UBuildingPreviewComponent::SetBuildable(bool bValue)
{
	bBuildable = bValue;

	if (buildingPreview)
	{
		// 머티리얼 파라미터: 1이면 빨강(불가), 0이면 초록(가능) 식으로 사용 중이었다고 가정
		buildingPreview->SetScalarParameterValue(TEXT("Buildable"), !bValue);
	}
}

void UBuildingPreviewComponent::OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{
	Super::OnUpdateTransform(UpdateTransformFlags, Teleport);
	if (bHiddenInGame || !TargetBuildingMesh)
	{
		return;
	}

	// 현재 위치에서의 겹침(배치 가능 여부)만 판정

	// 겹침 검사 대상 ObjectTypes
	TArray<TEnumAsByte<EObjectTypeQuery>> BuildCheckObjectTypes;
	BuildCheckObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	BuildCheckObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	BuildCheckObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));
	BuildCheckObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(GetOwner());

	TArray<FHitResult> PenetratingHits;

	FVector HalfSize = TargetBuildingMesh->GetBoundingBox().GetSize() * 0.5f;
	HalfSize.Z = (TargetBuildingMesh->GetBoundingBox().GetSize().Z - BottomTrans.GetLocation().Z)  * 0.5f;

	// 프리뷰 중심 위치(살짝 위로 올려서 박스 중심 정렬)
	const FVector Start = GetComponentLocation() + FVector(0.f, 0.f, HalfSize.Z) + FVector{0, 0, BottomTrans.GetLocation().Z};
	const FVector End = Start;

	const FRotator Rot = GetComponentRotation();

	bool bHit = UKismetSystemLibrary::BoxTraceMultiForObjects(
		GetWorld(),
		Start, End, HalfSize, Rot, 
		//UEngineTypes::ConvertToTraceType(ECC_Visibility),
		BuildCheckObjectTypes,
		true, IgnoreActors,
		EDrawDebugTrace::Type::ForDuration, PenetratingHits, true,
		FLinearColor::Black, FLinearColor::Green, 5.0f);
	bool bOvelap{};
	if (bHit)
	{
		for (const FHitResult& Hit : PenetratingHits)
		{
			// PenetrationDepth 값 문제로 중복 설치가 되는 문제가 있음
			if (Hit.bStartPenetrating &&
				Hit.PenetrationDepth > 80.f &&
				Hit.GetComponent() != this)
			{
				bResult = false;
				bOvelap = true;
				break;
			}
		}
	}
	if (!bOvelap)
		bResult = true;
	SetBuildable(bResult);
}
