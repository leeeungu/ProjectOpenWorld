#include "Building/BaseBuilding.h"
#include "Components/StaticMeshComponent.h"
#include "Building/Component/BuildingProgress.h"
#include "Building/Component/PalBuildingStaticMeshComponent.h"
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
	PalBuildingStaticMeshComponent = CreateDefaultSubobject<UPalBuildingStaticMeshComponent>(TEXT("PalBuildingStaticMeshComponent"));

	SetRootComponent(PalBuildingStaticMeshComponent);
	PalBuildingStaticMeshComponent->SetMobility(EComponentMobility::Static);
	//buildingMeshComponent->SetCanEverAffectNavigation(false);
	PalBuildingStaticMeshComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	PalBuildingStaticMeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	PalBuildingStaticMeshComponent->SetGenerateOverlapEvents(true);
	
	//buildingProgressComponent = CreateDefaultSubobject<UBuildingProgress>(TEXT("BuildingProgress"));
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

UStaticMeshComponent* ABaseBuilding::GetBuildingMeshComponent() const
{
	return PalBuildingStaticMeshComponent;
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
	if (!PalBuildingStaticMeshComponent)
		return true;
	return PalBuildingStaticMeshComponent->IsBuildingEnd();
}

void ABaseBuilding::NewGenerateWorldEvent(const FGenerateSectionData& SectionData)
{
	if (PalBuildingStaticMeshComponent->IsBuildingEnd())
		UpdateModifier();
}

void ABaseBuilding::DelGenerateWorldEvent(const FGenerateSectionData& SectionData)
{
	NoCollision();
	if (!PalBuildingStaticMeshComponent->IsBuildingEnd())
		PalBuildingStaticMeshComponent->StopAll();
}

void ABaseBuilding::UpdateModifier()
{
	if (!NavModifier)
		return;

///	UE_LOG(LogTemp, Warning, TEXT("ABaseBuilding::UpdateModifier"));
	PalBuildingStaticMeshComponent->SetCollisionProfileName(TEXT("P_Building"));
	PalBuildingStaticMeshComponent->SetMobility(EComponentMobility::Static);
	PalBuildingStaticMeshComponent->SetCanEverAffectNavigation(true);
	UNavigationSystemV1* Nav = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (Nav && PalBuildingStaticMeshComponent->GetStaticMesh())
		Nav->AddDirtyArea(PalBuildingStaticMeshComponent->GetStaticMesh()->GetBounds().GetBox(), 0);
	NavModifier->CalculateBounds();
	NavModifier->SetAreaClass(UNavArea_Default::StaticClass());
}

void ABaseBuilding::NoCollision()
{
	BuildActionWidget->SetCollisionProfileName(TEXT("NoCollision"));
	PalBuildingStaticMeshComponent->SetCanEverAffectNavigation(false);
	UNavigationSystemV1* Nav = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (Nav && PalBuildingStaticMeshComponent->GetStaticMesh())
		Nav->AddDirtyArea(PalBuildingStaticMeshComponent->GetStaticMesh()->GetBounds().GetBox(), 0);
	NavModifier->CalculateBounds();
	NavModifier->SetAreaClass(UNavArea_Default::StaticClass());
}
