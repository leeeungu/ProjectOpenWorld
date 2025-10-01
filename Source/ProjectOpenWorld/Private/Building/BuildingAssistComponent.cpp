#include "Building/BuildingAssistComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/StaticMeshActor.h" 
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Building/BaseBuilding.h"

UBuildingAssistComponent::UBuildingAssistComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	//Script/Engine.Material'/Game/Building/Material/M_BuildingPreview.M_BuildingPreview'
	ConstructorHelpers::FObjectFinder< UMaterial> PreviewMat(TEXT("/Game/Building/Material/M_BuildingPreview.M_BuildingPreview"));
	if (PreviewMat.Succeeded())
	{
		buildingPreviewMat = PreviewMat.Object;
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

		buildingPreview = UMaterialInstanceDynamic::Create(buildingPreviewMat.Get(), this);
	}
	if (GetOwner() && Cast< AController>(GetOwner()))
	{
		ownerPawn = Cast< AController>(GetOwner())->GetPawn();
	}
	OnOffAssist(false);
}

void UBuildingAssistComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	const APlayerCameraManager* CameraManager =
		UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (CameraManager && ownerPawn && buildingPreviewActor)
	{
		TArray<AActor*> Ignore{};
		Ignore.Reserve(2);
		Ignore.Add(ownerPawn.Get());
		Ignore.Add(buildingPreviewActor.Get());
		TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes{};
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
		FHitResult HitResult{};
		if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(),
			CameraManager->GetCameraLocation(),
			UKismetMathLibrary::GetForwardVector(CameraManager->GetCameraRotation()) * 1200 + CameraManager->GetCameraLocation(),
			ObjectTypes, true, Ignore, EDrawDebugTrace::Type::None, HitResult, true
		))
		{
			buildingPreviewActor->SetActorLocation(HitResult.Location);
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
			bool bSnap = false;
			for (auto& MeshPair : snapSocketTransform)
			{
				if (FVector::Dist(HitResult.ImpactPoint, MeshPair.Value.GetLocation()) <= 80.0f)
				{
					bSnap = true;
					snapSocketName = MeshPair.Key;
					buildingPreviewActor->SetActorLocation(MeshPair.Value.GetLocation());
					buildingPreviewActor->SetActorRotation(MeshPair.Value.GetRotation());
					break;
				}
			}
			if (!bSnap)
			{
				snapSocketName = NAME_None;
			}
			int Vaule = 0;
			canBuilding = true;
			ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
			ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));
			Ignore.Add(targetActor.Get());
			if (UKismetSystemLibrary::BoxTraceSingleForObjects(GetWorld(),
				meshCenter+buildingPreviewActor->GetActorLocation(),
				meshCenter+buildingPreviewActor->GetActorLocation(),
				meshSize, buildingPreviewActor->GetActorRotation(),
				ObjectTypes, true, Ignore, EDrawDebugTrace::Type::ForOneFrame, HitResult, true))
			{
				Vaule = 1;
				canBuilding = false;
			}
			if (buildingPreview)
				buildingPreview.Get()->SetScalarParameterValue(TEXT("Buildable"), Vaule);

		}
		else
		{
			targetActor = nullptr;
			if (buildingPreview)
				buildingPreview.Get()->SetScalarParameterValue(TEXT("Buildable"), 1);
			canBuilding = false;
		}
	}
	else
	{
		canBuilding = false;
		buildingPreview.Get()->SetScalarParameterValue(TEXT("Buildable"), 1);
	}
}

void UBuildingAssistComponent::SetBuildingStaticMesh(UStaticMesh* NewStaticMesh)
{
	if (!buildingActive)
	{
		OnOffAssist(true);
		buildingPreviewActor->GetStaticMeshComponent()->SetStaticMesh(NewStaticMesh);
		buildingPreviewActor->GetStaticMeshComponent()->SetMaterial(0, buildingPreview.Get());
		meshSize = NewStaticMesh->GetBoundingBox().GetExtent();
		meshCenter = NewStaticMesh->GetBoundingBox().GetCenter();
		if (buildingPreview)
			buildingPreview.Get()->SetScalarParameterValue(TEXT("Buildable"), 0);
	}
}

void UBuildingAssistComponent::SpawnBuilding()
{
	if (buildingActive && canBuilding)
	{
		OnOffAssist(false);
		FActorSpawnParameters Param{};
		Param.Instigator = ownerPawn.Get();
		Param.Owner = ownerPawn.Get();
		Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		//Param.Name = TEXT("NewBuilding");
		ABaseBuilding* Building = Cast< ABaseBuilding>(GetWorld()->SpawnActor(ABaseBuilding::StaticClass(), &buildingPreviewActor->GetTransform(), Param));
		buildingPreviewActor->SetActorRotation(FQuat::Identity);
		Building->SetbuildingMesh(buildingPreviewActor->GetStaticMeshComponent()->GetStaticMesh());
	}
}

void UBuildingAssistComponent::OnOffAssist(bool bValue)
{
	buildingActive = bValue;
	canBuilding = !bValue;
	SetComponentTickEnabled(bValue);
	if (buildingPreviewActor)
		buildingPreviewActor->SetActorHiddenInGame(!bValue);
}

