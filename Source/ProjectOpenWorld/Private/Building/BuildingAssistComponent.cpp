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
		}
	}
}

void UBuildingAssistComponent::SetBuildingStaticMesh(UStaticMesh* NewStaticMesh)
{
	if (!buildingActive)
	{
		OnOffAssist(true);
		buildingPreviewActor->GetStaticMeshComponent()->SetStaticMesh(NewStaticMesh);
		buildingPreviewActor->GetStaticMeshComponent()->SetMaterial(0, buildingPreviewMat.Get());
	}
}

void UBuildingAssistComponent::SpawnBuilding()
{
	if (buildingActive)
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
	SetComponentTickEnabled(bValue);
	if (buildingPreviewActor)
		buildingPreviewActor->SetActorHiddenInGame(!bValue);
}

