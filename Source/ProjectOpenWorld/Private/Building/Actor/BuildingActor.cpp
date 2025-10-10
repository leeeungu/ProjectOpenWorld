#include "Building/Actor/BuildingActor.h"
#include "Building/Component/BuildingProgress.h"

void ABuildingActor::OnBeginDetected_Implementation(APlayerController* pPlayer)
{
	if (!GetBuildingProgress()->IsAlreadyStart())
		GetBuildingProgress()->StartBuilding();
	GetBuildingProgress()->ResumeBuilding();
}

void ABuildingActor::OnEndDetected_Implementation(APlayerController* pPlayer)
{
	GetBuildingProgress()->StopBuilding();
}
