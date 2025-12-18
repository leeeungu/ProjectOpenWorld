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
		buildingPreviewActor->SetWorldTransform(PreviewWorld, false, nullptr, ETeleportType::ResetPhysics);
		return true;
	}

	// Hit된 Actor에서 ParentMesh 찾기

	ABaseBuilding* Building = Cast< ABaseBuilding>(HitResult.GetActor());
	if (!Building)
	{
		FVector Bottom =  buildingPreviewActor->GetSocketLocation(TEXT("Bottom")) - buildingPreviewActor->GetComponentLocation();
		PreviewWorld.SetLocation(HitResult.ImpactPoint - Bottom);// +FVector{ 0,0,400 });
		PreviewWorld.SetRotation(CurrentRot.Quaternion());
		PreviewWorld.SetScale3D(CurrentScale);
		buildingPreviewActor->SetWorldTransform(PreviewWorld, false, nullptr, ETeleportType::ResetPhysics);
		return true;
	}
	UStaticMeshComponent* ParentMeshComp = Building->GetBuildingMeshComponent();
	
	
	const FVector ImpactPoint = HitResult.ImpactPoint - BottomTrans.GetLocation();

	// ParentMesh가 없으면 ImpactPoint에 자유 배치
	if (!ParentMeshComp || !BuildingMesh.IsValid() || SnapRulesForChild.Num() == 0) // NotSnap, hitPosition
	{
		PreviewWorld.SetLocation(ImpactPoint);
		PreviewWorld.SetRotation(CurrentRot.Quaternion());
		PreviewWorld.SetScale3D(CurrentScale);
		buildingPreviewActor->SetWorldTransform(PreviewWorld, false, nullptr, ETeleportType::ResetPhysics);
		return true;
	}

	// Parent StaticMesh 기준으로 스냅 룰 필터링
	UStaticMesh* ParentStaticMesh = ParentMeshComp->GetStaticMesh();
	if (!ParentStaticMesh)
	{
		PreviewWorld.SetLocation(ImpactPoint);
		PreviewWorld.SetRotation(CurrentRot.Quaternion());
		PreviewWorld.SetScale3D(CurrentScale);
		buildingPreviewActor->SetWorldTransform(PreviewWorld, false, nullptr, ETeleportType::ResetPhysics);
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
	if (!MainRule || !MainRule->ParentMesh) // !Snaped
	{
		PreviewWorld.SetLocation(ImpactPoint);
		PreviewWorld.SetRotation(CurrentRot.Quaternion());
		PreviewWorld.SetScale3D(CurrentScale);
		buildingPreviewActor->SetWorldTransform(PreviewWorld, false, nullptr, ETeleportType::ResetPhysics);
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
	const FVector Extention = { 400,400,400 };
	FVector BestLocation{};
	FRotator BestRotator{};
	//FVector BestScale{};
	FVector PrentAnchor{};

	for (const FSnapAnchorData& Data : MainRule->ArrayAnchors)
	{
		//MainRule->ParentAnchorLocal.GetLocation();
		FTransform ParentAnchorWorld = MainRule->ParentAnchorLocal * ParentWorld;
		FVector AnchorWorldPos = ParentAnchorWorld.GetLocation();
		FVector PreviewLocation = ImpactPoint;
		FRotator PreviewRotator = ParentAnchorWorld.GetRotation().Rotator();

		if (Data.ParentAnchor != ESnapAnchor::NONE)
		{
			FVector ParentOffset = FSnapRule::AnchorToOffset(Data.ParentAnchor, Extention); //MainRule->ParentMesh->GetBoundingBox().GetSize()); // Extention
			//) ; // FVector(1, -1, 1)
			FVector ChildOffset = FSnapRule::AnchorToOffset(Data.ChildAnchor, Extention)  +Data.ChildOffset;

			ParentAnchorWorld = HitResult.GetActor()->GetTransform();
			AnchorWorldPos = ParentAnchorWorld.TransformPosition(ParentOffset);
			PreviewRotator += FRotator(0, FSnapRule::AnchorYaw(Data.ChildYaw), 0);
			const FVector RotatedChildAnchor = PreviewRotator.Quaternion() * ChildOffset;
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
				BestLocation = PreviewLocation;
				BestRotator = PreviewRotator;
				PrentAnchor = AnchorWorldPos;
			}
		}
		else if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			BestRule = MainRule;
			BestParentAnchorWorld = ParentAnchorWorld;
			BestIndex = Index;
			Min = dot;
			BestLocation = PreviewLocation;
			BestRotator = PreviewRotator;
			PrentAnchor = AnchorWorldPos;
		}

		Index++;
	}
	//UE_LOG(LogTemp, Warning, TEXT("%s"),  *Temp);

	// Anchor가 너무 멀면 스냅하지 않고 ImpactPoint로 자유 배치
	if (!BestRule)// || BestDistSq > SnapDistSq)
	{
		PreviewWorld.SetLocation(ImpactPoint);
		PreviewWorld.SetRotation(CurrentRot.Quaternion());
		PreviewWorld.SetScale3D(CurrentScale);

		buildingPreviewActor->SetWorldTransform(PreviewWorld, false, nullptr, ETeleportType::ResetPhysics);
		return true;
	}
	if (BestRule->ArrayAnchors.IsValidIndex(BestIndex))
	{
		CurrentScale = BestParentAnchorWorld.GetScale3D();
		PreviewWorld.SetLocation(BestLocation);
		PreviewWorld.SetRotation(BestRotator.Quaternion());
		PreviewWorld.SetScale3D(CurrentScale);
		if (FVector::DistSquared(PrePoint, HitResult.ImpactPoint) >= SnapDistance * SnapDistance)
		{
			buildingPreviewActor->SetWorldTransform(PreviewWorld, false, nullptr, ETeleportType::ResetPhysics);
			PrePoint = PrentAnchor;
		}
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
