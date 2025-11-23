#include "Building/BaseBuilding.h"
#include "Components/StaticMeshComponent.h"
#include "Building/Component/BuildingProgress.h"
#include "Building/Component/BuildingActionWidgetComponent.h"
#include "NavModifierComponent.h"
#include "NavAreas/NavArea_Default.h"

ABaseBuilding::ABaseBuilding()
{
	PrimaryActorTick.bCanEverTick = false;
	//mobility(EComponentMobility::Static);
	buildingMeshComponent = CreateDefaultSubobject< UStaticMeshComponent>(TEXT("BuildingMesh"));
	SetRootComponent(buildingMeshComponent);
	buildingMeshComponent->SetMobility(EComponentMobility::Static);
	buildingMeshComponent->SetCanEverAffectNavigation(false);
	buildingMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	buildingMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	
	buildingProgressComponent = CreateDefaultSubobject<UBuildingProgress>(TEXT("BuildingProgress"));
	BuildActionWidget = CreateDefaultSubobject<UBuildingActionWidgetComponent>(TEXT("BuildActionWidget"));
	if (BuildActionWidget)
	{
		BuildActionWidget->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	}

	
	NavModifier = CreateDefaultSubobject<UNavModifierComponent>(TEXT("NavModifier"));
	NavModifier->SetAreaClass(UNavArea_Default::StaticClass());
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