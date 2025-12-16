#include "Building/Component/BuildingAssistComponentV2.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Building/Component/BuildingPreviewComponent.h"
#include "Building/BaseBuilding.h"

bool UBuildingAssistComponentV2::UpdatePreview()
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

	ABaseBuilding* Building = Cast< ABaseBuilding>(HitResult.GetActor());
	if (!Building)
		return false;

	UStaticMeshComponent* ParentMeshComp = Building->GetBuildingMeshComponent();
	
	
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
	FVector NewLocation{};
	//FString Temp{};
	for (const FSnapAnchorData& Data : MainRule->ArrayAnchors)
	{
		FVector Extention = MainRule->ParentAnchorLocal.GetLocation();
		FTransform ParentAnchorWorld = MainRule->ParentAnchorLocal * ParentWorld;
		FVector AnchorWorldPos = ParentAnchorWorld.GetLocation();
		FVector PreviewLocation = ImpactPoint;

		if (Data.ParentAnchor != ESnapAnchor::NONE)
		{
			FVector ParentOffset = FSnapRule::AnchorToOffset(Data.ParentAnchor, Extention) ; // FVector(1, -1, 1)
			FVector ChildOffset = FSnapRule::AnchorToOffset(Data.ChildAnchor, Extention) + Data.ChildOffset ;

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
				NewLocation = PreviewLocation;
			}
		}
		else if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			BestRule = MainRule;
			BestParentAnchorWorld = ParentAnchorWorld;
			BestIndex = Index;
			Min = dot;
			NewLocation = PreviewLocation;
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