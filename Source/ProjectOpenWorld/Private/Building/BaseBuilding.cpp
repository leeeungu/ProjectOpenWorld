#include "Building/BaseBuilding.h"
#include "Components/StaticMeshComponent.h"
#include "Building/Component/BuildingProgress.h"
#include "Building/Component/BuildingActionWidgetComponent.h"

ABaseBuilding::ABaseBuilding()
{
	PrimaryActorTick.bCanEverTick = false;
	Root = CreateDefaultSubobject< USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	//mobility(EComponentMobility::Static);
	Root->SetMobility(EComponentMobility::Static);
	buildingMeshComponent = CreateDefaultSubobject< UStaticMeshComponent>(TEXT("BuildingMesh"));
	buildingMeshComponent->SetMobility(EComponentMobility::Static);
	buildingMeshComponent->SetupAttachment(Root);
	buildingMeshComponent->SetCanEverAffectNavigation(false);
	buildingMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	buildingMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	
	buildingProgressComponent = CreateDefaultSubobject<UBuildingProgress>(TEXT("BuildingProgress"));
	BuildActionWidget = CreateDefaultSubobject<UBuildingActionWidgetComponent>(TEXT("BuildActionWidget"));
	if (BuildActionWidget)
	{
		BuildActionWidget->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
	}
}

void ABaseBuilding::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseBuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseBuilding::OnConstruction(const FTransform& Transform)
{
	AActor::OnConstruction(Transform);
}