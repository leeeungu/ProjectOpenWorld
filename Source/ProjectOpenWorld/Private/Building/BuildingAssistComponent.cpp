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
	canBuilding = false;
	if (ownerPawn && buildingPreviewActor)
	{
		FVector MoveLocation = buildingPreviewActor->GetActorLocation();

		if (UpdatePreviewMesh(MoveLocation))
		{
			canBuilding = true;
		}
		MoveLocation -= FVector(0, 0, meshCenter.Z + meshSize.Z);
		if (bSnapped && targetActor)
		{
			MoveLocation -= FVector(0, 0, meshSize.Z );
			FVector dir = (MoveLocation - targetActor->GetActorLocation());
			dir = FVector(dir.X, dir.Y, 0).GetSafeNormal();
			MoveLocation += dir * meshSize;
		}
		buildingPreviewActor->SetActorLocation(MoveLocation);
	}
	UpdatePreviewMat();
}

void UBuildingAssistComponent::SetBuildingStaticMesh(UStaticMesh* NewStaticMesh)
{
	BuildingMesh = NewStaticMesh;
	buildingPreviewActor->GetStaticMeshComponent()->SetStaticMesh(BuildingMesh.Get());
	int nSize = buildingPreviewActor->GetStaticMeshComponent()->GetMaterials().Num();
	for (int i = 0; i < nSize; i++)
	{
		buildingPreviewActor->GetStaticMeshComponent()->SetMaterial(i, buildingPreview.Get());
	}
	meshSize = BuildingMesh->GetBoundingBox().GetExtent();
	FTransform Relative = buildingPreviewActor->GetStaticMeshComponent()->GetSocketTransform(TEXT("BuildingCenter"), ERelativeTransformSpace::RTS_Component);
	meshCenter = Relative.GetLocation();
	UE_LOG(LogTemp, Warning, TEXT("%s"), *meshSize.ToString());
	meshSize.Z = (meshSize.Z  * 2- meshCenter.Z) * 0.5f;
	buildingPreviewActor->SetActorRelativeLocation(meshCenter * -1);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *meshSize.ToString());
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
		FVector Location = Trans.GetLocation();
		Trans.SetLocation(FVector(Location.X, Location.Y, 0));
		ABaseBuilding* Building = Cast< ABaseBuilding>(GetWorld()->SpawnActor(BuildingClass, &Trans, Param));
		buildingPreviewActor->SetActorRotation(FQuat::Identity);
		if (Building)
		{
			Building->GetBuildingMeshComponent()->SetMobility(EComponentMobility::Movable);
			Building->GetBuildingMeshComponent()->SetRelativeLocation({0,0,Location.Z});
			Building->GetBuildingMeshComponent()->SetMobility(EComponentMobility::Static);
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
		if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(),
			CameraManager->GetCameraLocation(),
			UKismetMathLibrary::GetForwardVector(CameraManager->GetCameraRotation()) * 1200 + CameraManager->GetCameraLocation(),
			buildPointObjectTypes, true, buildPointIgnore, EDrawDebugTrace::Type::None, HitResult, true))
		{
			ResultPoint = HitResult.ImpactPoint;
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

			//buildCheckIgnore.Last() = targetActor.Get();

			FVector Size = meshSize;
			Size.Z = Size.Z * 0.5f;
			FVector MoveLocation = ResultPoint + FVector(0, 0, Size.Z);
			if (bSnapped)
			{
				MoveLocation = ResultPoint - FVector(0, 0, Size.Z);
				FVector dir = (MoveLocation - targetActor->GetActorLocation());
				dir = FVector(dir.X, dir.Y, 0).GetSafeNormal();
				MoveLocation += dir * Size;
			}

			
			TArray<FHitResult> ArrayPenetratingResult{};
			if (UKismetSystemLibrary::BoxTraceMultiForObjects(GetWorld(),
				MoveLocation,
				MoveLocation, //buildingPreviewActor->GetActorLocation(),
				Size, buildingPreviewActor->GetActorRotation(),
				buildCheckObjectTypes, true, buildCheckIgnore, EDrawDebugTrace::Type::ForOneFrame, ArrayPenetratingResult, true, FLinearColor::Black))
			{
				for (const FHitResult& PenetratingResult : ArrayPenetratingResult)
				{
					// 지형 제외 겹치는 지 채크하는 코드
					if (PenetratingResult.bStartPenetrating && !Cast<ALandscapeProxy>(PenetratingResult.GetActor()) && PenetratingResult.PenetrationDepth > 10.0f)
					{
						UE_LOG(LogTemp, Error, TEXT("Hit Actor %s"), *PenetratingResult.GetActor()->GetFName().ToString());
						return false;
					}
				}
			}

			//double Angle = FMath::RadiansToDegrees(FMath::Acos(HitResult.ImpactNormal.Dot(FVector::UpVector)));
			//if (!bSnap && Angle > 15.0)
			//{
			//	return false;
			//}
			return true;
		}
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

