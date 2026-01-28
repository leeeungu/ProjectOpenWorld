#include "Building/BuildingAssistComponent.h"

#include "Building/Component/BuildingPreviewComponent.h"
#include "Building/BaseBuilding.h"
#include "Building/Component/BuildingProgress.h"
#include "Building/Widget/BuildingInfoWidget.h"

#include "Engine/StaticMeshActor.h"
#include "Engine/DataTable.h"
#include "GameFramework/Pawn.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#include "LandscapeProxy.h"
#include "Blueprint/UserWidget.h"

#include "Player/Controller/BasePlayerController.h"
#include "Inventory/Component/InventoryComponent.h"
#include "Building/Subsystem/BuildingDataSubsystem.h"

UBuildingAssistComponent::UBuildingAssistComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// 안내 위젯
	static ConstructorHelpers::FClassFinder<UUserWidget> BuildingWidget(
		TEXT("/Game/Building/Widget/WBP_BuildGuidInfo.WBP_BuildGuidInfo_C"));
	if (BuildingWidget.Succeeded())
	{
		BuildingInfoClass = BuildingWidget.Class;
	}

	// 기본 빌딩 Actor 클래스
	static ConstructorHelpers::FClassFinder<ABaseBuilding> BuildingBP(
		TEXT("/Game/Building/Blueprints/Test/Bp_BuildingActor.Bp_BuildingActor_C"));
	if (BuildingBP.Succeeded())
	{
		BuildingClass = BuildingBP.Class;
	}

	// Snap DataTable 로딩
	static ConstructorHelpers::FObjectFinder<UDataTable> MeshData(
		TEXT("/Game/Building/Data/DT_SnapData.DT_SnapData"));
	if (MeshData.Succeeded())
	{
		if (UDataTable* DataTable = MeshData.Object)
		{
			DataTable->GetAllRows<FSnapRule>(TEXT("SnapData"), SnapDataRows);
		}
	}

	// Trace용 ObjectTypes
	buildPointObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	buildPointObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	buildPointObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel1));
	EndBuilding();
}

void UBuildingAssistComponent::BeginPlay()
{
	UActorComponent::BeginPlay();

	if (GetOwner())
	{
		ownerPawn = Cast<APawn>(GetOwner());
	}

	if (ownerPawn.IsValid())
	{
		// 프리뷰 컴포넌트 생성
		buildingPreviewActor = Cast<UBuildingPreviewComponent>(
			ownerPawn->AddComponentByClass(UBuildingPreviewComponent::StaticClass(), false, FTransform::Identity, false));

		if (buildingPreviewActor)
		{
			buildingPreviewActor->SetRelativeLocation(FVector(400.f, 0.f, 0.f));
		}
	}

	// Trace Ignore: 소유자
	buildPointIgnore.Add(GetOwner());

	// 빌딩 안내 위젯 생성
	if (BuildingInfoClass)
	{
		BuildingInfo = Cast<UBuildingInfoWidget>(CreateWidget(GetWorld(), BuildingInfoClass));
	}
}

void UBuildingAssistComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	UActorComponent::TickComponent(DeltaTime, TickType, ThisTickFunction);

	canBuilding = false;

	if (!buildingActive || !ownerPawn.IsValid() || !buildingPreviewActor)
	{
		return;
	}

	// 카메라 Trace + 스냅 + Preview 이동
	if (UpdatePreview())
	{
		// PreviewComponent 가 OnUpdateTransform 에서 겹침 판정을 끝냈다고 가정
		canBuilding = buildingPreviewActor->IsBuildable();
	}
	else
		canBuilding = false;
}

void UBuildingAssistComponent::SetBuildingStaticMesh(FName BuildingID, UStaticMesh* NewStaticMesh)
{
	if (!NewStaticMesh || !buildingPreviewActor)
	{
		return;
	}
	CurrentBuildingID = BuildingID;
	BuildingMesh = NewStaticMesh;
	buildingPreviewActor->SetBuildingMsh(BuildingMesh.Get());

	BottomTrans = buildingPreviewActor->GetSocketTransform(TEXT("Bottom"), ERelativeTransformSpace::RTS_Component);
	// 현재 ChildMesh 에 맞는 SnapRule들만 추려둠
	SnapRulesForChild.Reset();
	for (FSnapRule* Row : SnapDataRows)
	{
		if (Row && Row->ChildMesh == BuildingMesh.Get())
		{
			SnapRulesForChild.Add(Row);
		}
	}
}

void UBuildingAssistComponent::StartBuilding()
{
	OnOffAssist(true);

	if (BuildingInfo)
	{
		BuildingInfo->SetBuildingInfoData(CurrentBuildingID);
		BuildingInfo->AddToViewport();
	}

	if (buildingPreviewActor)
	{
		buildingPreviewActor->StartPreView();
	}
}

void UBuildingAssistComponent::EndBuilding()
{
	OnOffAssist(false);

	if (BuildingInfo)
	{
		BuildingInfo->RemoveFromParent();
	}

	if (buildingPreviewActor)
	{
		buildingPreviewActor->EndPreView();
	}
}

void UBuildingAssistComponent::SpawnBuilding()
{
	if (!buildingActive || !canBuilding || !BuildingClass || !buildingPreviewActor || !BuildingMesh.IsValid() || !ownerPawn.Get())
	{
		return;
	}
	ABasePlayerController* PC = Cast<ABasePlayerController>(ownerPawn->GetController());
	if (PC)
	{
		if (UInventoryComponent* Inventory = PC->GetInventoryComponent())
		{
			if (UBuildingDataSubsystem::IsValidInstance())
			{
				const FPalBuildObjectData* BuildData = nullptr;
				UBuildingDataSubsystem::GetPalBuildObjectData(CurrentBuildingID, BuildData);
				if (BuildData)
				{
					BuildData->Materials;
					for (const FBuildMaterialData& Material : BuildData->Materials)
					{
						if (!Inventory->HasItem(Material.Material_Id, Material.Material_Count))
						{
							// 재료 부족
							return;
						}
					}

					for (const FBuildMaterialData& Material : BuildData->Materials)
					{
						Inventory->RemoveItem(Material.Material_Id, Material.Material_Count);
					}
				}
			}
		}
	}
	if (BuildingInfo)
	{
		BuildingInfo->UpdateBuildingInfo();
	}
	FActorSpawnParameters Param;
	Param.Instigator = ownerPawn.Get();
	Param.Owner = ownerPawn.Get();
	Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	FTransform SpawnTransform{};
	SpawnTransform .SetLocation(buildingPreviewActor->GetComponentLocation());
	SpawnTransform .SetRotation(buildingPreviewActor->GetComponentRotation().Quaternion());
	SpawnTransform .SetScale3D(buildingPreviewActor->GetComponentScale());

	ABaseBuilding* Building = GetWorld()->SpawnActor<ABaseBuilding>(BuildingClass, SpawnTransform, Param);
	if (Building)
	{
		if (UBuildingProgress* Progress = Building->GetBuildingProgress())
		{
			Progress->SetbuildingMesh(BuildingMesh.Get());
		}
	}

	// 프리뷰 회전 초기화
	buildingPreviewActor->SetWorldRotation(FQuat::Identity);
}

void UBuildingAssistComponent::RotateBuilding(float AddYaw)
{
	if (buildingPreviewActor)
	{
		buildingPreviewActor->AddWorldRotation(FRotator(0.f, AddYaw, 0.f));
		YawRotation = UKismetMathLibrary::NormalizeAxis(YawRotation + AddYaw);
	}
}

void UBuildingAssistComponent::OnOffAssist(bool bValue)
{
	buildingActive = bValue;
	canBuilding = !bValue;
	SetComponentTickEnabled(bValue);

	if (!buildingPreviewActor)
	{
		return;
	}

	if (bValue)
	{
		buildingPreviewActor->StartPreView();
	}
	else
	{
		buildingPreviewActor->EndPreView();
		CurrentBuildingID = NAME_None;
	}
}

bool UBuildingAssistComponent::UpdatePreview()
{
	if (!ownerPawn.IsValid() || !buildingPreviewActor)
	{
		return false;
	}

	const APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (!CameraManager)
	{
		return false;
	}

	const FVector CamLocation = CameraManager->GetCameraLocation();
	const FRotator CamRotation = CameraManager->GetCameraRotation();
	const FVector TraceStart = CamLocation;
	const FVector TraceEnd = CamLocation + CamRotation.Vector() * 1200.f - BottomTrans.GetLocation();

	FHitResult HitResult;
	bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(
		GetWorld(),
		TraceStart,
		TraceEnd,
		buildPointObjectTypes,
		true,
		buildPointIgnore,
		EDrawDebugTrace::Type::None,
		HitResult,
		true);

	FTransform PreviewWorld = buildingPreviewActor->GetComponentTransform();

	// 기본 회전은 기존 프리뷰의 회전 유지 (RotateBuilding으로 조절)
	FRotator  CurrentRot = PreviewWorld.GetRotation().Rotator();
	//FRotator TO FQUAT
	CurrentRot.Yaw = YawRotation;
	FVector CurrentScale = FVector::OneVector;

	if (!bHit) // no hit
	{
		// 아무 것도 맞지 않으면 TraceEnd에 자유 배치
		PreviewWorld.SetLocation(TraceEnd);
		PreviewWorld.SetRotation(CurrentRot.Quaternion());
		PreviewWorld.SetScale3D(CurrentScale);

		buildingPreviewActor->SetWorldTransform(PreviewWorld, false, nullptr, ETeleportType::TeleportPhysics);
		return true;
	}
	// Hit된 Actor에서 ParentMesh 찾기
	UStaticMeshComponent* ParentMeshComp = nullptr;
	if (AActor* HitActor = HitResult.GetActor())
	{
		for (UActorComponent* Comp : HitActor->GetComponents())
		{
			if (UStaticMeshComponent* SMC = Cast<UStaticMeshComponent>(Comp))
			{
				if (SMC != buildingPreviewActor)
				{
					ParentMeshComp = SMC;
					break;
				}
			}
		}
	}
	const FVector ImpactPoint = HitResult.ImpactPoint - BottomTrans.GetLocation();

	// ParentMesh가 없으면 ImpactPoint에 자유 배치
	if (!ParentMeshComp || !BuildingMesh.IsValid() || SnapRulesForChild.Num() == 0) // NotSnap, hitPosition
	{
		PreviewWorld.SetLocation(ImpactPoint);
		PreviewWorld.SetRotation(CurrentRot.Quaternion());
		PreviewWorld.SetScale3D(CurrentScale);

		buildingPreviewActor->SetWorldTransform(PreviewWorld, false, nullptr, ETeleportType::TeleportPhysics);
		return true;
	}

	// Parent StaticMesh 기준으로 스냅 룰 필터링
	UStaticMesh* ParentStaticMesh = ParentMeshComp->GetStaticMesh();
	if (!ParentStaticMesh) 
	{
		PreviewWorld.SetLocation(ImpactPoint);
		PreviewWorld.SetRotation(CurrentRot.Quaternion());
		PreviewWorld.SetScale3D(CurrentScale);

		buildingPreviewActor->SetWorldTransform(PreviewWorld, false, nullptr, ETeleportType::TeleportPhysics);
		return true;
	}

	const FSnapRule* MainRule{};
	for (const FSnapRule* Rule : SnapRulesForChild)
	{
		if (Rule && Rule->ParentMesh == ParentStaticMesh)
		{
			MainRule = Rule;
			break;
		}
	}

	// 이 Parent에 대한 스냅 룰이 없으면 ImpactPoint에 자유 배치
	if (!MainRule) // !Snaped
	{
		PreviewWorld.SetLocation(ImpactPoint);
		PreviewWorld.SetRotation(CurrentRot.Quaternion());
		PreviewWorld.SetScale3D(CurrentScale);

		buildingPreviewActor->SetWorldTransform(PreviewWorld, false, nullptr, ETeleportType::TeleportPhysics);
		return true;
	}

	// 가장 가까운 ParentAnchorWorld 찾기
	const FTransform ParentWorld = ParentMeshComp->GetComponentTransform();
	const float SnapDistSq = SnapDistance * SnapDistance;
	const FSnapRule* BestRule = nullptr;
	int Index{};
	int BestIndex{};
	float BestDistSq = TNumericLimits<float>::Max();
	FTransform BestParentAnchorWorld;
	float Min = TNumericLimits<float>::Max();
	//FString Temp{};
	for (const FSnapAnchorData& Data : MainRule->ArrayAnchors)
	{
		FTransform ParentAnchorWorld = MainRule->ParentAnchorLocal * ParentWorld;
		FVector AnchorWorldPos = ParentAnchorWorld.GetLocation();
		FVector PreviewLocation = ImpactPoint;

		if (Data.ParentAnchor != ESnapAnchor::NONE)
		{
			FVector ParentOffset = FSnapRule::AnchorToOffset(Data.ParentAnchor, MainRule->ParentMesh->GetBoundingBox().GetSize()); // FVector(1, -1, 1)
			FVector ChildOffset = FSnapRule::AnchorToOffset(Data.ChildAnchor, MainRule->ChildMesh->GetBoundingBox().GetSize()) + Data.ChildOffset;

			ParentAnchorWorld = HitResult.GetActor()->GetTransform();
			AnchorWorldPos = ParentAnchorWorld.TransformPosition(ParentOffset);
			const FVector RotatedChildAnchor = ParentAnchorWorld.GetRotation() * ChildOffset;
			PreviewLocation = AnchorWorldPos - RotatedChildAnchor;

		}

		const float DistSq = FVector::DistSquared(ImpactPoint, AnchorWorldPos);
		const float dot = FVector::DotProduct((PreviewLocation - AnchorWorldPos).GetSafeNormal(), (GetOwner()->GetActorLocation() - AnchorWorldPos).GetSafeNormal());
		if (FMath::IsNearlyEqual(DistSq, BestDistSq))
		{
			if (dot > Min)
			{
				BestDistSq = DistSq;
				BestRule = MainRule;
				BestParentAnchorWorld = ParentAnchorWorld;
				BestIndex = Index;
				Min = dot;
			}
		}
		else if (DistSq < BestDistSq )
		{
			BestDistSq = DistSq;
			BestRule = MainRule;
			BestParentAnchorWorld = ParentAnchorWorld;
			BestIndex = Index;
			Min = dot;
		}
	
		Index++;
	}
	//UE_LOG(LogTemp, Warning, TEXT("%s"),  *Temp);

	// Anchor가 너무 멀면 스냅하지 않고 ImpactPoint로 자유 배치
	if (!BestRule || BestDistSq > SnapDistSq)
	{
		PreviewWorld.SetLocation(ImpactPoint);
		PreviewWorld.SetRotation(CurrentRot.Quaternion());
		PreviewWorld.SetScale3D(CurrentScale);

		buildingPreviewActor->SetWorldTransform(PreviewWorld, false, nullptr, ETeleportType::TeleportPhysics);
		return true;
	}
	if (BestRule->ArrayAnchors.IsValidIndex(BestIndex))
	{
		CurrentScale = BestParentAnchorWorld.GetScale3D();
		const FSnapAnchorData& Data = BestRule->ArrayAnchors[BestIndex];
		FVector ParentOffset = FSnapRule::AnchorToOffset(Data.ParentAnchor, MainRule->ParentMesh->GetBoundingBox().GetSize());;
		FVector ChildOffset = FSnapRule::AnchorToOffset(Data.ChildAnchor, MainRule->ChildMesh->GetBoundingBox().GetSize()) + Data.ChildOffset;

		// 회전 포함 Parent Anchor World 좌표
		const FVector ParentAnchorWorld = BestParentAnchorWorld.TransformPosition(ParentOffset * CurrentScale);

		// 회전 보정: ChildAnchorLocal을 Parent 기준으로 회전
		const FVector RotatedChildAnchor = BestParentAnchorWorld.GetRotation() * (ChildOffset * CurrentScale);

		// 최종 위치 = ParentAnchorWorld - (회전된 ChildAnchorLocal)
		//const FVector ChildWorldPos = ParentAnchorWorld - RotatedChildAnchor;

		PreviewWorld.SetLocation(ParentAnchorWorld - RotatedChildAnchor);

		CurrentRot = BestParentAnchorWorld.GetRotation().Rotator();
		CurrentRot.Yaw += FSnapRule::AnchorYaw(Data.ChildYaw);
		PreviewWorld.SetRotation(CurrentRot.Quaternion());
		PreviewWorld.SetScale3D(CurrentScale);
	}
	else
	{
		// 스냅: ChildWorld * ChildAnchorLocal = ParentAnchorWorld
		const FTransform& ChildAnchorLocal = BestRule->ChildAnchorLocal;

		const FQuat ParentAnchorWorldRot = BestParentAnchorWorld.GetRotation();
		const FQuat ChildAnchorLocalRot = ChildAnchorLocal.GetRotation();

		const FQuat ChildWorldRot = ParentAnchorWorldRot * ChildAnchorLocalRot.Inverse();

		const FVector ParentAnchorWorldPos = BestParentAnchorWorld.GetLocation();
		const FVector ChildAnchorLocalPos = ChildAnchorLocal.GetLocation();

		const FVector ChildAnchorWorldOffset = ChildWorldRot.RotateVector(ChildAnchorLocalPos);
		const FVector ChildWorldPos = ParentAnchorWorldPos - ChildAnchorWorldOffset;

		PreviewWorld.SetLocation(ChildWorldPos);
		PreviewWorld.SetRotation(ChildWorldRot);
		PreviewWorld.SetScale3D(CurrentScale);
	}

	buildingPreviewActor->SetWorldTransform(PreviewWorld, false, nullptr, ETeleportType::ResetPhysics);
	return true;
}
