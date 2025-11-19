#include "Building/BuildingAssistComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/StaticMeshActor.h" 
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Building/BaseBuilding.h"
#include "LandscapeProxy.h"
#include "GameFramework/Pawn.h"
#include "Building/Component/BuildingProgress.h"
#include "Building/Widget/BuildingInfoWidget.h"

UBuildingAssistComponent::UBuildingAssistComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	// //Script/Engine.Material'/Game/Building/Mesh/Material/M_BuildingPreview.M_BuildingPreview'
	static ConstructorHelpers::FObjectFinder< UMaterial> PreviewMat(TEXT("/Game/Building/Mesh/Material/M_BuildingPreview.M_BuildingPreview"));
	if (PreviewMat.Succeeded())
	{
		buildingPreviewMat = PreviewMat.Object;
	}
	//Script/UMGEditor.WidgetBlueprint'/Game/Building/Widget/WBP_BuildGuidInfo.WBP_BuildGuidInfo'
	static ConstructorHelpers::FClassFinder< UUserWidget> BuildingWidget(TEXT("/Game/Building/Widget/WBP_BuildGuidInfo.WBP_BuildGuidInfo_C"));
	if (BuildingWidget.Succeeded())
	{
		BuildingInfoClass = BuildingWidget.Class;
	}
	//SetIsReplicated(true);

	//Script/Engine.Blueprint'/Game/Building/Blueprints/Test/Bp_BuildingActor.Bp_BuildingActor'
	static ConstructorHelpers::FClassFinder< ABaseBuilding> Building(TEXT("/Game/Building/Blueprints/Test/Bp_BuildingActor.Bp_BuildingActor_C"));
	if (Building.Succeeded())
	{
		BuildingClass = Building.Class;
	}
	ArraySnapSocket.Reserve(50);
}


void UBuildingAssistComponent::BeginPlay()
{
	Super::BeginPlay();
	buildingPreviewActor = GetWorld()->SpawnActor<AStaticMeshActor>();
	if (buildingPreviewActor)
	{
		buildingPreviewActor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		buildingPreviewActor->SetMobility(EComponentMobility::Type::Movable);
		buildPointIgnore.Add(buildingPreviewActor.Get());
		buildingPreview = UMaterialInstanceDynamic::Create(buildingPreviewMat.Get(), this);
	}
	if (GetOwner() && Cast< APawn>(GetOwner()))
	{
		ownerPawn = Cast< APawn>(GetOwner());
	//	buildPointIgnore.Add(ownerPawn.Get());
	}
	OnOffAssist(false);

	buildPointObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	buildPointObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	buildCheckObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	buildCheckObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	buildCheckObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));
	buildCheckObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	buildCheckIgnore = buildPointIgnore;
	buildPointIgnore.Add(GetOwner());
	buildCheckIgnore.Add(nullptr);
	if (BuildingInfoClass)
		BuildingInfo =Cast< UBuildingInfoWidget>(CreateWidget(GetWorld(), BuildingInfoClass));
}

void UBuildingAssistComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	FHitResult Hit;
	bool bHit{};

	if (const APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0))
	{
		//TArray<FHitResult> ArraygResult{};
		FHitResult HitResult{};
		// 어떤 물체와 충돌하는 지
		if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(),
			CameraManager->GetCameraLocation(),
			UKismetMathLibrary::GetForwardVector(CameraManager->GetCameraRotation()) * 1200 + CameraManager->GetCameraLocation(),
			buildPointObjectTypes, true, buildPointIgnore, EDrawDebugTrace::Type::None, HitResult, true))
		{
			bHit = true;
		}
	}

	if (!bHit)
	{
		// 충돌 없으면 Ray 끝 지점에 프리뷰 배치 (자유 배치)
		// DoPlacementTrace 안에서 End 지정했었다고 가정
		// 여기서는 Hit 실패 시 End 위치를 따로 리턴하도록 구현해도 됨.
		// 예시는 카메라 앞 일정 거리라고 가정.

		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC)
		{
			const FVector CamLoc = PC->PlayerCameraManager->GetCameraLocation();
			const FRotator CamRot = PC->PlayerCameraManager->GetCameraRotation();
			const FVector End = CamLoc + CamRot.Vector() * 5000.f;

			FTransform FreeTransform;
			FreeTransform.SetLocation(Hit.TraceEnd);
			FreeTransform.SetRotation(FQuat::Identity);
			FreeTransform.SetScale3D(FVector::OneVector);

			buildingPreviewActor->SetActorTransform(FreeTransform);
		}
	}
	else
	{
		// Hit된 컴포넌트에서 스냅 시도
		UStaticMeshComponent* HitSMC = Cast<UStaticMeshComponent>(Hit.GetComponent());
		if (!HitSMC || !HitSMC->GetStaticMesh())
		{
			// StaticMesh가 아니면 그냥 충돌 지점에 자유 배치
			FTransform FreeTransform;
			FreeTransform.SetLocation(Hit.TraceEnd);
			FreeTransform.SetRotation(FQuat::Identity);
			FreeTransform.SetScale3D(FVector::OneVector);
			buildingPreviewActor->SetActorTransform(FreeTransform);
			return;
		}
		else
		{
			// 스냅 시도
			FTransform SnapTransform{};
			bSnapped = FindBestSnapTransform(HitSMC, Hit.ImpactPoint, SnapTransform);

			if (bSnapped)
			{
				buildingPreviewActor->SetActorTransform(SnapTransform);
				bSnapped = CanPlaceAtTransform(SnapTransform);
			}
			else
			{
				// 해당 ParentMesh 에 대해 CurrentBuildMesh 를 붙일 룰이 없으니
				// 그냥 ImpactPoint 에 자유 배치
				FTransform FreeTransform;
				FreeTransform.SetLocation(Hit.ImpactPoint);
				FreeTransform.SetRotation(FQuat::Identity);
				FreeTransform.SetScale3D(FVector::OneVector);

				buildingPreviewActor->SetActorTransform(FreeTransform);
			}
		}
	}
	UpdatePreviewMat();
}

bool UBuildingAssistComponent::FindBestSnapTransform(
	UStaticMeshComponent* ParentComp,
	const FVector& HitWorldLocation,
	FTransform& OutChildWorldTransform) const
{
	if (!ParentComp || !ParentComp->GetStaticMesh() || !CurrentBuildMesh)
	{
		return false;
	}

	UStaticMesh* ParentMesh = ParentComp->GetStaticMesh();

	// 1) Parent/Child 조합에 맞는 룰들 필터링
	TArray<const FSnapRule*> CandidateRules;
	for (const FSnapRule& Rule : SnapRules)
	{
		if (Rule.ParentMesh == ParentMesh && Rule.ChildMesh == CurrentBuildMesh)
		{
			CandidateRules.Add(&Rule);
		}
	}

	if (CandidateRules.Num() == 0)
	{
		return false;
	}

	const FTransform ParentWorld = ParentComp->GetComponentTransform();

	// 2) HitWorldLocation 을 Parent 로컬 공간으로 변환
	const FVector HitLocal = ParentWorld.InverseTransformPosition(HitWorldLocation);

	// 3) 가장 가까운 ParentAnchorLocal 위치를 가진 룰 선택
	const FSnapRule* BestRule = nullptr;
	float BestDistSq = TNumericLimits<float>::Max();

	for (const FSnapRule* RulePtr : CandidateRules)
	{
		const FVector AnchorLocalPos = RulePtr->ParentAnchorLocal.GetLocation();
		const float DistSq = FVector::DistSquared(HitLocal, AnchorLocalPos);

		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			BestRule = RulePtr;
		}
	}

	if (!BestRule)
	{
		return false;
	}

	if (BestDistSq > FMath::Square(SnapLocalDistanceThreshold))
	{
		// 너무 멀면 스냅하지 않음
		return false;
	}

	// 4) ParentAnchorWorld 계산
	const FTransform& ParentAnchorLocal = BestRule->ParentAnchorLocal;
	const FTransform& ChildAnchorLocal = BestRule->ChildAnchorLocal;

	const FTransform ParentAnchorWorld = ParentAnchorLocal * ParentWorld;

	// 5) ChildWorldTransform 역산
	// ChildWorld * ChildAnchorLocal = ParentAnchorWorld
	// → 회전, 위치 순서대로 풀기

	const FQuat ParentAnchorWorldRot = ParentAnchorWorld.GetRotation();
	const FQuat ChildAnchorLocalRot = ChildAnchorLocal.GetRotation();

	// 회전: ChildWorldRot = ParentAnchorWorldRot * Inverse(ChildAnchorLocalRot)
	const FQuat ChildWorldRot = ParentAnchorWorldRot * ChildAnchorLocalRot.Inverse();

	// 위치:
	// ParentAnchorWorldPos = ChildWorldPos + ChildWorldRot * ChildAnchorLocalPos
	// → ChildWorldPos = ParentAnchorWorldPos - ChildWorldRot * ChildAnchorLocalPos
	const FVector ParentAnchorWorldPos = ParentAnchorWorld.GetLocation();
	const FVector ChildAnchorLocalPos = ChildAnchorLocal.GetLocation();

	const FVector ChildAnchorWorldOffset = ChildWorldRot.RotateVector(ChildAnchorLocalPos);
	const FVector ChildWorldPos = ParentAnchorWorldPos - ChildAnchorWorldOffset;

	OutChildWorldTransform.SetLocation(ChildWorldPos);
	OutChildWorldTransform.SetRotation(ChildWorldRot);
	OutChildWorldTransform.SetScale3D(FVector::OneVector); // 필요시 Scale도 룰에 넣어서 계산

	return true;
}
//
//	canBuilding = false;
//	if (ownerPawn && buildingPreviewActor)
//	{
//		FVector MoveLocation = buildingPreviewActor->GetActorLocation();
//
//		if (UpdatePreviewMesh(MoveLocation) && targetActor)
//		{
//			// 스냅 이거나 물체랑 충돌 시에만 실행
//			canBuilding = true;
//			FVector Location = MoveLocation;
//			// 스냅
//			if (bSnapped)
//			{
//				FVector dir = (MoveLocation - (targetActor->GetActorLocation() + meshCenter));
//				if (FMath::IsNearlyZero(dir.X))
//					dir.X = 0;
//				if (dir.X > 0)
//					dir.X = 1;
//				else if (dir.X < 0)
//					dir.X = -1;
//
//				if (FMath::IsNearlyZero(dir.Y))
//					dir.Y = 0;
//				if (dir.Y > 0)
//					dir.Y = 1;
//				else if (dir.Y < 0)
//					dir.Y = -1;
//
//				if (FMath::IsNearlyZero(dir.Z))
//					dir.Z = 0;
//				if (dir.Z > 0)
//					dir.Z = 1;
//				else if (dir.Z < 0)
//					dir.Z = -1;
//
//
//				Location = targetActor->GetActorLocation() + dir * meshSize;// +meshCenter;
//				MoveLocation = Location;
//				FVector Size = meshSize * 0.5f;
//				//if (!FMath::IsNearlyEqual(Size.Z,  meshCenter.Z * 0.5F))
//				//	Size.Z = (Size.Z - meshCenter.Z * 0.5F);
//				TArray<FHitResult> ArrayPenetratingResult{};
//				if (UKismetSystemLibrary::BoxTraceMultiForObjects(GetWorld(),
//					Location + FVector{0,0,meshSize.Z} * 0.5f,
//					Location + FVector{0, 0, meshSize.Z} *0.5f, //buildingPreviewActor->GetActorLocation(),
//					Size, buildingPreviewActor->GetActorRotation(),
//					buildCheckObjectTypes, true, buildCheckIgnore, EDrawDebugTrace::Type::ForOneFrame, ArrayPenetratingResult, true, FLinearColor::Black))
//				{
//					for (const FHitResult& PenetratingResult : ArrayPenetratingResult)
//					{
//						// 지형 제외 겹치는 지 채크하는 코드
//						if (PenetratingResult.bStartPenetrating && !Cast<ALandscapeProxy>(PenetratingResult.GetActor()) && PenetratingResult.PenetrationDepth > 10.0f)
//						{
//							//UE_LOG(LogTemp, Error, TEXT("Hit Actor %s"), *PenetratingResult.GetActor()->GetFName().ToString());
//							canBuilding = false;
//							break;
//						}
//					}
//				}
//			}
//			else
//			{
//				MoveLocation -= FVector(0, 0, meshCenter.Z);
//
//			}
//			//if (bSnapped && targetActor)
//			//{
//			//	FVector dir = (MoveLocation - targetActor->GetActorLocation());
//			//	MoveLocation = targetActor->GetActorLocation();
//			//	dir = dir.GetSafeNormal();
//			//	MoveLocation += dir * meshSize * 2;
//			//}
//			//canBuilding = true;
//			//FVector Size = meshSize;
//			////Size.Z = Size.Z * 0.5f;
//			//if (bSnapped)
//			//{
//			//	FVector dir = (Location - targetActor->GetActorLocation());
//			//	Location = targetActor->GetActorLocation() + meshCenter;
//			//	dir = dir.GetSafeNormal();
//			//	Location += dir * meshSize * 2;
//			//}
//		}
//		
//		buildingPreviewActor->SetActorLocation(MoveLocation);
//	}
//	UpdatePreviewMat();
//}

void UBuildingAssistComponent::SetBuildingStaticMesh(UStaticMesh* NewStaticMesh)
{
	BuildingMesh = NewStaticMesh;
	buildingPreviewActor->GetStaticMeshComponent()->SetStaticMesh(BuildingMesh.Get());
	int nSize = buildingPreviewActor->GetStaticMeshComponent()->GetMaterials().Num();
	for (int i = 0; i < nSize; i++)
	{
		buildingPreviewActor->GetStaticMeshComponent()->SetMaterial(i, buildingPreview.Get());
	}
	meshSize = BuildingMesh->GetBoundingBox().GetSize();
	FTransform Relative = buildingPreviewActor->GetStaticMeshComponent()->GetSocketTransform(TEXT("BuildingCenter"), ERelativeTransformSpace::RTS_Component);
	meshCenter = Relative.GetLocation();
	//meshSize.Z = abs(meshCenter.Z * 0.25f);
	//buildingPreviewActor->SetActorRelativeLocation(meshCenter * -1);
	UE_LOG(LogTemp, Warning, TEXT("meshCenter : %s"), *meshCenter.ToString());
	UE_LOG(LogTemp, Warning, TEXT("MeshSize : %s"), *meshSize.ToString());
		//buildingPreviewActor->GetStaticMeshComponent()->SetRelativeLocation({ 0,0,meshSize.Z });
	UpdatePreviewMat();
}

void UBuildingAssistComponent::StartBuilding()
{
	OnOffAssist(true);
	if (BuildingInfo)
		BuildingInfo->AddToViewport();
	//FAttachmentTransformRules Rule(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, false);
	//buildingPreviewActor->AttachToActor(ownerPawn.Get(), Rule);
}

void UBuildingAssistComponent::EndBuilding()
{
	//UE_LOG(LogTemp, Error, TEXT("EndBuilding"));
	OnOffAssist(false);
	if (BuildingInfo)	
		BuildingInfo->RemoveFromParent();
}

void UBuildingAssistComponent::SpawnBuilding()
{
	if (buildingActive && canBuilding)
	{
		FActorSpawnParameters Param{};
		Param.Instigator = ownerPawn.Get();
		Param.Owner = ownerPawn.Get();
		Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		FTransform Trans = buildingPreviewActor->GetTransform();
		ABaseBuilding* Building = Cast< ABaseBuilding>(GetWorld()->SpawnActor(BuildingClass, &Trans, Param));
		buildingPreviewActor->SetActorRotation(FQuat::Identity);
		if (Building)
		{
			UBuildingProgress* Progress = Building->GetBuildingProgress();
			Building->GetBuildingProgress()->SetbuildingMesh(BuildingMesh.Get());
		}
		//Building->GetBuildingProgress()->StartBuilding();
	}
}

void UBuildingAssistComponent::RotateBuilding(float AddYaw)
{
	if (buildingPreviewActor)
		buildingPreviewActor->AddActorWorldRotation(FRotator{0,AddYaw,0});
}

void UBuildingAssistComponent::OnOffAssist(bool bValue)
{
	buildingActive = bValue;
	canBuilding = !bValue;
	SetComponentTickEnabled(bValue);
	if (buildingPreviewActor)
		buildingPreviewActor->SetActorHiddenInGame(!bValue);
}

bool UBuildingAssistComponent::UpdateTraceHit(FHitResult& HitResult)
{
	if (const APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0))
	{
		if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(),
			CameraManager->GetCameraLocation(), 
			UKismetMathLibrary::GetForwardVector(CameraManager->GetCameraRotation()) * 1200 + CameraManager->GetCameraLocation(),
			buildPointObjectTypes, true, buildPointIgnore, EDrawDebugTrace::Type::None, HitResult, true))
		{
			if (targetActor != HitResult.GetActor())
			{
				targetActor = HitResult.GetActor();
				snapSocketTransform.Empty(5);
				for (auto& Component : targetActor->GetComponents())
				{
					UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(Component);
					if (Mesh)
					{
						for (auto Socket : Mesh->GetAllSocketNames())
						{
							if (Socket.ToString().Contains(TEXT("BuildingPoint")))
							{
								snapSocketTransform.Add(Socket, Mesh->GetSocketTransform(Socket));
							}
						}
					}
				}
			}
			return true;
		}
	}
	return false;
}

bool UBuildingAssistComponent::UpdateSnap(FVector& ResultPoint)
{
	bool bSnap = false;
	for (auto& MeshPair : snapSocketTransform)
	{
		if (FVector::DistSquared(ResultPoint, MeshPair.Value.GetLocation()) <= 40.0f* 40.0f)
		{
			bSnap = true;
			snapSocketName = MeshPair.Key;
			ResultPoint = MeshPair.Value.GetLocation();
			buildingPreviewActor->SetActorRotation(MeshPair.Value.GetRotation());
			break;
		}
	}
	if (!bSnap)
	{
		snapSocketName = NAME_None;
	}
	return bSnap;
}

bool UBuildingAssistComponent::UpdatePreviewMesh(FVector& ResultPoint)
{
	if (const APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0))
	{
		//TArray<FHitResult> ArraygResult{};
		FHitResult HitResult{};
		// 어떤 물체와 충돌하는 지
		if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(),
			CameraManager->GetCameraLocation(),
			UKismetMathLibrary::GetForwardVector(CameraManager->GetCameraRotation()) * 1200 + CameraManager->GetCameraLocation(),
			buildPointObjectTypes, true, buildPointIgnore, EDrawDebugTrace::Type::None, HitResult, true)) 
		{
			ResultPoint = HitResult.ImpactPoint;
			// 충돌 시 소캣이 있는지
			if (targetActor != HitResult.GetActor() && targetActor != GetOwner())
			{
				targetActor = HitResult.GetActor();
				//snapSocketTransform.Empty(5);
				ArraySnapSocket.Empty();
				for (auto& Component : targetActor->GetComponents())
				{
					UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(Component);
					if (Mesh)
					{
						for (auto Socket : Mesh->GetAllSocketNames())
						{
							if (Socket.ToString().Contains(TEXT("BuildingPoint")))
							{
								ArraySnapSocket.Add({ Mesh, Socket });
							}
						}
					}
				}
			}

			// 충동 시 소캣에 스냅 되는 지
			bSnapped = false;
			buildCheckIgnore.Last() = nullptr;
			if (targetActor == HitResult.GetActor() && targetActor != GetOwner())
			{
				if (!ArraySnapSocket.IsEmpty())
				{
					for (auto& Component : ArraySnapSocket)
					{
						FTransform SnapTransform = Component.Key->GetSocketTransform(Component.Value);
						if (FVector::DistSquared(ResultPoint, Component.Key->GetSocketLocation(Component.Value)) <= 80.0f * 80.0f)
						{
							bSnapped = true;
							snapSocketName = Component.Value;
							ResultPoint = SnapTransform.GetLocation();
							buildingPreviewActor->SetActorRotation(SnapTransform.GetRotation());
							buildCheckIgnore.Last() = targetActor.Get();
							break;
						}
					}
				}
			}
			return true;
		}
		ResultPoint = HitResult.TraceEnd;
	}

	return false;
}

bool UBuildingAssistComponent::UpdateBuildable()
{
	bool bResult = true;
	buildCheckIgnore.Last() = targetActor.Get();
	TArray<FHitResult> ArrayPenetratingResult{};
	meshCenter = buildingPreviewActor->GetStaticMeshComponent()->GetSocketLocation(TEXT("BuildingCenter")) +FVector(0, 0, meshSize.Z);
	if (UKismetSystemLibrary::BoxTraceMultiForObjects(GetWorld(),
		meshCenter, //buildingPreviewActor->GetActorLocation(),
		meshCenter, //buildingPreviewActor->GetActorLocation(),
		meshSize, buildingPreviewActor->GetActorRotation(),
		buildCheckObjectTypes, true, buildCheckIgnore, EDrawDebugTrace::Type::ForOneFrame, ArrayPenetratingResult, true, FLinearColor::Black))
	{
		for (const FHitResult& PenetratingResult : ArrayPenetratingResult)
		{
			if (PenetratingResult.bStartPenetrating && !Cast<ALandscapeProxy>(PenetratingResult.GetActor()) && PenetratingResult.PenetrationDepth > 5.0f)
			{
				bResult = false;
				break;
			}
		}
	}
	return bResult;
}

void UBuildingAssistComponent::UpdatePreviewMat()
{
	if (buildingPreview)
		buildingPreview.Get()->SetScalarParameterValue(TEXT("Buildable"), !canBuilding);
}

