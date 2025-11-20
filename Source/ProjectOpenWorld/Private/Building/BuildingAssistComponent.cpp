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
	if (GetOwner() && Cast< APawn>(GetOwner()))
	{
		ownerPawn = Cast< APawn>(GetOwner());
	//	buildPointIgnore.Add(ownerPawn.Get());
	}
	buildingPreviewActor = Cast< UBuildingPreviewComponent>(ownerPawn->GetComponentByClass<UBuildingPreviewComponent>());
	buildingPreviewActor->SetRelativeLocation({ 400,0,0 });

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
	canBuilding = false;
	if (ownerPawn && buildingPreviewActor)
	{
		FVector MoveLocation = buildingPreviewActor->GetComponentLocation();

		if (UpdatePreviewMesh(MoveLocation) && targetActor)
		{
			// 스냅 이거나 물체랑 충돌 시에만 실행
			canBuilding = true;
			FVector Location = MoveLocation;
			// 스냅
			if (bSnapped)
			{
				FVector dir = (MoveLocation - (targetActor->GetActorLocation() + meshCenter));
				if (FMath::IsNearlyZero(dir.X))
					dir.X = 0;
				if (dir.X > 0)
					dir.X = 1;
				else if (dir.X < 0)
					dir.X = -1;

				if (FMath::IsNearlyZero(dir.Y))
					dir.Y = 0;
				if (dir.Y > 0)
					dir.Y = 1;
				else if (dir.Y < 0)
					dir.Y = -1;

				if (FMath::IsNearlyZero(dir.Z))
					dir.Z = 0;
				if (dir.Z > 0)
					dir.Z = 1;
				else if (dir.Z < 0)
					dir.Z = -1;


				Location = targetActor->GetActorLocation() + dir * meshSize;// +meshCenter;
				MoveLocation = Location;
				FVector Size = meshSize * 0.5f;
				//if (!FMath::IsNearlyEqual(Size.Z,  meshCenter.Z * 0.5F))
				//	Size.Z = (Size.Z - meshCenter.Z * 0.5F);
				//TArray<FHitResult> ArrayPenetratingResult{};
				//if (UKismetSystemLibrary::BoxTraceMultiForObjects(GetWorld(),
				//	Location + FVector{0,0,meshSize.Z} * 0.5f,
				//	Location + FVector{0, 0, meshSize.Z} *0.5f, //buildingPreviewActor->GetActorLocation(),
				//	Size, buildingPreviewActor->GetComponentRotation(),
				//	buildCheckObjectTypes, true, buildCheckIgnore, EDrawDebugTrace::Type::ForOneFrame, ArrayPenetratingResult, true, FLinearColor::Black))
				//{
				//	for (const FHitResult& PenetratingResult : ArrayPenetratingResult)
				//	{
				//		// 지형 제외 겹치는 지 채크하는 코드
				//		if (PenetratingResult.bStartPenetrating && !Cast<ALandscapeProxy>(PenetratingResult.GetActor()) && PenetratingResult.PenetrationDepth > 10.0f)
				//		{
				//			//UE_LOG(LogTemp, Error, TEXT("Hit Actor %s"), *PenetratingResult.GetActor()->GetFName().ToString());
				//			canBuilding = false;
				//			break;
				//		}
				//	}
				//}
			}
			else
			{
				MoveLocation -= FVector(0, 0, meshCenter.Z);

			}
			if (bSnapped && targetActor)
			{
				FVector dir = (MoveLocation - targetActor->GetActorLocation());
				MoveLocation = targetActor->GetActorLocation();
				dir = dir.GetSafeNormal();
				MoveLocation += dir * meshSize * 2;
			}
			canBuilding = true;
			FVector Size = meshSize;
			//Size.Z = Size.Z * 0.5f;
			if (bSnapped)
			{
				FVector dir = (Location - targetActor->GetActorLocation());
				Location = targetActor->GetActorLocation() + meshCenter;
				dir = dir.GetSafeNormal();
				Location += dir * meshSize * 2;
			}
		}
		
		buildingPreviewActor->SetWorldLocation(MoveLocation);
	}
}

void UBuildingAssistComponent::SetBuildingStaticMesh(UStaticMesh* NewStaticMesh)
{
	if (!NewStaticMesh)
		return;
	BuildingMesh = NewStaticMesh;
	buildingPreviewActor->SetBuildingMsh(BuildingMesh.Get());
}

void UBuildingAssistComponent::StartBuilding()
{
	OnOffAssist(true);
	if (BuildingInfo)
		BuildingInfo->AddToViewport();
	buildingPreviewActor->StartPreView();

	//FAttachmentTransformRules Rule(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, false);
	//buildingPreviewActor->AttachToActor(ownerPawn.Get(), Rule);
}

void UBuildingAssistComponent::EndBuilding()
{
	//UE_LOG(LogTemp, Error, TEXT("EndBuilding"));
	OnOffAssist(false);
	if (BuildingInfo)	
		BuildingInfo->RemoveFromParent();
	buildingPreviewActor->EndPreView();
}

void UBuildingAssistComponent::SpawnBuilding()
{
	if (buildingActive && canBuilding)
	{
		FActorSpawnParameters Param{};
		Param.Instigator = ownerPawn.Get();
		Param.Owner = ownerPawn.Get();
		Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		FTransform Trans = buildingPreviewActor->GetComponentTransform();
		ABaseBuilding* Building = Cast< ABaseBuilding>(GetWorld()->SpawnActor(BuildingClass, &Trans, Param));
		buildingPreviewActor->SetWorldRotation(FQuat::Identity);
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
		buildingPreviewActor->AddWorldRotation(FRotator{0,AddYaw,0});
}

void UBuildingAssistComponent::OnOffAssist(bool bValue)
{
	buildingActive = bValue;
	canBuilding = !bValue;
	SetComponentTickEnabled(bValue);

	if (bValue)
		buildingPreviewActor->StartPreView();
	else
		buildingPreviewActor->EndPreView();
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
			buildingPreviewActor->SetWorldRotation(MeshPair.Value.GetRotation());
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
							buildingPreviewActor->SetWorldRotation(SnapTransform.GetRotation());
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
	meshCenter = buildingPreviewActor->GetSocketLocation(TEXT("BuildingCenter")) +FVector(0, 0, meshSize.Z);
	if (UKismetSystemLibrary::BoxTraceMultiForObjects(GetWorld(),
		meshCenter, //buildingPreviewActor->GetActorLocation(),
		meshCenter, //buildingPreviewActor->GetActorLocation(),
		meshSize, buildingPreviewActor->GetComponentRotation(),
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
}

