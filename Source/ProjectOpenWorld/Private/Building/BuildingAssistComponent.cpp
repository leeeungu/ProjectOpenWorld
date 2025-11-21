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
		FTransform ChildWorld = buildingPreviewActor->GetComponentTransform();
		FVector MoveLocation = ChildWorld.GetLocation();
		if (!UpdatePreviewMesh(MoveLocation))
		{
			{
				ChildWorld.SetLocation(MoveLocation);
				buildingPreviewActor->SetWorldTransform(ChildWorld, false, nullptr, ETeleportType::TeleportPhysics);
				buildingPreviewActor->Modify();
			}
		}
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
}

void UBuildingAssistComponent::EndBuilding()
{
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


bool UBuildingAssistComponent::UpdatePreviewMesh(FVector& ResultPoint)
{
	UStaticMeshComponent* Mesh{};
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
				bSnapped = false;
				for (auto& Component : targetActor->GetComponents())
				{
					Mesh = Cast<UStaticMeshComponent>(Component);
					if (Mesh)
					{
						bSnapped = true;
						break;;
					}
				}
			}
		}
		if (!Mesh)
			ResultPoint = HitResult.TraceEnd;
		buildingPreviewActor->SetParentMesh(HitResult, Mesh);
	}
	return Mesh != nullptr;
}