#include "Building/Actor/BuildingActor.h"
#include "Building/Component/BuildingProgress.h"
#include "Building/Subsystem/BuildingWidgetSubsystem.h"

void ABuildingActor::BeginPlay()
{
	Super::BeginPlay();
}

void ABuildingActor::OnBeginDetected_Implementation(APlayerController* pPlayer)
{
	if (!pPlayer || !pPlayer->GetLocalPlayer())
		return;
	if (UBuildingWidgetSubsystem* BuildingWidgetSubsystem = pPlayer->GetLocalPlayer()->GetSubsystem<UBuildingWidgetSubsystem>()) // GetSubsystem가 Map에서 찾으니 괜찮은 듯
	{
		BuildingWidgetSubsystem->SetBuildingWidgetProperty(GetBuildingProgress());
		if (!GetBuildingProgress()->IsBuildingEnd())
		{
			BuildingWidgetSubsystem->AddBuildTimeWidget();
		}
	}
}

void ABuildingActor::OnEndDetected_Implementation(APlayerController* pPlayer)
{
	if (!pPlayer || !pPlayer->GetLocalPlayer())
		return;
	if (UBuildingWidgetSubsystem* BuildingWidgetSubsystem = pPlayer->GetLocalPlayer()->GetSubsystem<UBuildingWidgetSubsystem>())
	{
		BuildingWidgetSubsystem->RemoveBuildTimeWidget();
	}
	//GetBuildingProgress()->StopBuilding();
}

void ABuildingActor::OnInteractionStart_Implementation(APlayerController* pPlayer)
{
	GetBuildingProgress()->StartBuilding();
}

void ABuildingActor::OnInteraction_Implementation()
{
	GetBuildingProgress()->ResumeBuilding();
}

void ABuildingActor::OnInteractionEnd_Implementation(APlayerController* pPlayer)
{
	GetBuildingProgress()->StopBuilding();
}
