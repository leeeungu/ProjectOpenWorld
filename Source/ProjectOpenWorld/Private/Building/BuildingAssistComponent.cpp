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
		FHitResult HitResult{};
		FVector MoveLocation{};
		if (UpdateTraceHit(HitResult)) // 화면 중앙으로 RayCast를 쏴 건축 가능한 거리를 판별
		{
			MoveLocation = HitResult.Location;
			bool bSnap = UpdateSnap(MoveLocation); // 해당 위치에서 소켓이 있는 메시를 탐색
			canBuilding = UpdateBuildable(); // 최종적으로 빌드 가능한지 확인
			// 빌드 가능한 경사각인지 확인
			double Angle = FMath::RadiansToDegrees(FMath::Acos(HitResult.ImpactNormal.Dot(FVector::UpVector)));
			if (!bSnap && Angle > 15.0)
			{
				canBuilding = false;
			}
		}
		else
		{
			MoveLocation = HitResult.TraceEnd;
			targetActor = nullptr;
		}
		buildingPreviewActor->SetActorLocation(MoveLocation);
	}
	UpdatePreviewMat();
}

void UBuildingAssistComponent::SetBuildingStaticMesh(UStaticMesh* NewStaticMesh)
{
	buildingPreviewActor->GetStaticMeshComponent()->SetStaticMesh(NewStaticMesh);
	int nSize = buildingPreviewActor->GetStaticMeshComponent()->GetMaterials().Num();
	for (int i = 0; i < nSize; i++)
	{
		buildingPreviewActor->GetStaticMeshComponent()->SetMaterial(i, buildingPreview.Get());
	}
	meshSize = NewStaticMesh->GetBoundingBox().GetExtent();
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
		ABaseBuilding* Building = Cast< ABaseBuilding>(GetWorld()->SpawnActor(BuildingClass, &buildingPreviewActor->GetTransform(), Param));
		buildingPreviewActor->SetActorRotation(FQuat::Identity);

		if (Building)
		{
			UBuildingProgress* Progress = Building->GetBuildingProgress();
			Building->GetBuildingProgress()->SetbuildingMesh(buildingPreviewActor->GetStaticMeshComponent()->GetStaticMesh());
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

bool UBuildingAssistComponent::UpdateBuildable()
{
	bool bResult = true;
	buildCheckIgnore.Last() = targetActor.Get();
	TArray<FHitResult> ArrayPenetratingResult{};
	meshCenter = buildingPreviewActor->GetStaticMeshComponent()->GetSocketLocation(TEXT("BuildingCenter"))+ FVector(0,0,meshSize.Z);
	if (UKismetSystemLibrary::BoxTraceMultiForObjects(GetWorld(),
		meshCenter, //buildingPreviewActor->GetActorLocation(),
		meshCenter, //buildingPreviewActor->GetActorLocation(),
		meshSize, buildingPreviewActor->GetActorRotation(),
		buildCheckObjectTypes, true, buildCheckIgnore, EDrawDebugTrace::Type::ForOneFrame, ArrayPenetratingResult, true))
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

