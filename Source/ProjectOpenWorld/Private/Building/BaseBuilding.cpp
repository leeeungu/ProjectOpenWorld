#include "Building/BaseBuilding.h"
#include "Components/StaticMeshComponent.h"
#include "Building/Component/BuildingProgress.h"
#include "Building/Component/BuildingActionWidgetComponent.h"
#include "NavModifierComponent.h"
#include "NavAreas/NavArea_Obstacle.h"
#include "NavAreas/NavArea_Default.h"
#include "Pal/Factory/PalCommandFunctionLibrary.h"
#include "NavigationSystem.h"

ABaseBuilding::ABaseBuilding() :Super()
{
	PrimaryActorTick.bCanEverTick = false;
	//mobility(EComponentMobility::Static);
	buildingMeshComponent = CreateDefaultSubobject< UStaticMeshComponent>(TEXT("BuildingMesh"));
	SetRootComponent(buildingMeshComponent);
	buildingMeshComponent->SetMobility(EComponentMobility::Static);
	//buildingMeshComponent->SetCanEverAffectNavigation(false);
	buildingMeshComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	buildingMeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	buildingMeshComponent->SetGenerateOverlapEvents(true);
	
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

void ABaseBuilding::BeginDestroy()
{
	Super::BeginDestroy();
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

bool ABaseBuilding::IsCommandFinished_Implementation()
{
	if (!buildingProgressComponent)
		return true;
	return buildingProgressComponent->IsBuildingEnd();
}

void ABaseBuilding::NewGenerateWorldEvent(const FGenerateSectionData& SectionData)
{
	if (buildingProgressComponent->IsBuildingEnd())
		UpdateModifier();
}

void ABaseBuilding::DelGenerateWorldEvent(const FGenerateSectionData& SectionData)
{
	NoCollision();
	if (!buildingProgressComponent->IsBuildingEnd())
		buildingProgressComponent->StopAll();
}

void ABaseBuilding::UpdateModifier()
{
	if (!NavModifier)
		return;

///	UE_LOG(LogTemp, Warning, TEXT("ABaseBuilding::UpdateModifier"));
	buildingMeshComponent->SetCollisionProfileName(TEXT("P_Building"));
	buildingMeshComponent->SetMobility(EComponentMobility::Static);
	buildingMeshComponent->SetCanEverAffectNavigation(true);
	UNavigationSystemV1* Nav = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (Nav && buildingMeshComponent->GetStaticMesh())
		Nav->AddDirtyArea(buildingMeshComponent->GetStaticMesh()->GetBounds().GetBox(), 0);
	NavModifier->CalculateBounds();
	NavModifier->SetAreaClass(UNavArea_Default::StaticClass());
}

void ABaseBuilding::NoCollision()
{
	BuildActionWidget->SetCollisionProfileName(TEXT("NoCollision"));
	buildingMeshComponent->SetCanEverAffectNavigation(false);
	UNavigationSystemV1* Nav = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (Nav && buildingMeshComponent->GetStaticMesh())
		Nav->AddDirtyArea(buildingMeshComponent->GetStaticMesh()->GetBounds().GetBox(), 0);
	NavModifier->CalculateBounds();
	NavModifier->SetAreaClass(UNavArea_Default::StaticClass());
}
