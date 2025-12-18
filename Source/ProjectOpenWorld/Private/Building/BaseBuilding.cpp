#include "Building/BaseBuilding.h"
#include "Components/StaticMeshComponent.h"
#include "Building/Component/BuildingProgress.h"
#include "Building/Component/BuildingActionWidgetComponent.h"
#include "NavModifierComponent.h"
#include "NavAreas/NavArea_Obstacle.h"
#include "NavAreas/NavArea_Default.h"
#include "Pal/Factory/PalCommandFunctionLibrary.h"

ABaseBuilding::ABaseBuilding()
{
	PrimaryActorTick.bCanEverTick = false;
	//mobility(EComponentMobility::Static);
	buildingMeshComponent = CreateDefaultSubobject< UStaticMeshComponent>(TEXT("BuildingMesh"));
	SetRootComponent(buildingMeshComponent);
	buildingMeshComponent->SetMobility(EComponentMobility::Static);
	buildingMeshComponent->SetCanEverAffectNavigation(false);
	buildingMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	buildingMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);
	buildingMeshComponent->SetCanEverAffectNavigation(true);
	
	buildingProgressComponent = CreateDefaultSubobject<UBuildingProgress>(TEXT("BuildingProgress"));
	BuildActionWidget = CreateDefaultSubobject<UBuildingActionWidgetComponent>(TEXT("BuildActionWidget"));
	if (BuildActionWidget)
	{

		BuildActionWidget->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		BuildActionWidget->SetCollisionProfileName(TEXT("NoCollision"));
	}

	
	NavModifier = CreateDefaultSubobject<UNavModifierComponent>(TEXT("NavModifier"));
	NavModifier->CalculateBounds();
	NavModifier->SetAreaClass(UNavArea_Default::StaticClass());
	Command = UPalCommandFunctionLibrary::CommandArchitecture(nullptr, this);
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

EPalCommandKind ABaseBuilding::GetCommandKind_Implementation()
{
	return Command.CommandKind;
}

uint8 ABaseBuilding::GetSubCommandType_Implementation()
{
	return Command.SubCommandType;
}

FPalCommand ABaseBuilding::GetCommand_Implementation()
{
	return Command;
}

void ABaseBuilding::UpdateModifier()
{
	if (!NavModifier)
		return;
	NavModifier->CalculateBounds();
	NavModifier->SetAreaClass(UNavArea_Default::StaticClass());
}
