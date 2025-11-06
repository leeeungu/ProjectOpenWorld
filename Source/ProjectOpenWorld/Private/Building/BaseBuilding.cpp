#include "Building/BaseBuilding.h"
#include "Components/StaticMeshComponent.h"
#include "Building/Component/BuildingProgress.h"
#include "Building/Component/BuildingActionWidgetComponent.h"

ABaseBuilding::ABaseBuilding()
{
	PrimaryActorTick.bCanEverTick = false;
	buildingMeshComponent = CreateDefaultSubobject< UStaticMeshComponent>("BuildingMesh");
	if (buildingMeshComponent)
	{
		buildingMeshComponent->SetupAttachment(RootComponent);
		buildingMeshComponent->SetCanEverAffectNavigation(false);
		buildingMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
		buildingMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	}
	buildingProgressComponent = CreateDefaultSubobject<UBuildingProgress>(TEXT("BuildingProgress"));
	BuildActionWidget = CreateDefaultSubobject<UBuildingActionWidgetComponent>(TEXT("BuildActionWidget"));
	if (BuildActionWidget)
	{
		BuildActionWidget->SetupAttachment(GetRootComponent());
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