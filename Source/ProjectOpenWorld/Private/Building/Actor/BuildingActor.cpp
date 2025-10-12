#include "Building/Actor/BuildingActor.h"
#include "Building/Component/BuildingProgress.h"
#include "Building/Widget/BuildingStateWidget.h"

void ABuildingActor::BeginPlay()
{
	Super::BeginPlay();
}

void ABuildingActor::OnBeginDetected_Implementation(APlayerController* pPlayer)
{
	if (!GetBuildingProgress()->IsAlreadyStart())
	{
		if (!buildingStateWidget && buildingStateWidgetClass && pPlayer)
		{
			buildingStateWidget = Cast< UBuildingStateWidget>(CreateWidget(pPlayer, buildingStateWidgetClass));
			buildingStateWidget->SetPercent(GetBuildingProgress()->GetBuildPercent());
			buildingStateWidget->SetBuildTime(GetBuildingProgress()->GetBuildTime());
			GetBuildingProgress()->onBuildingEnd.AddDynamic(buildingStateWidget, &UBuildingStateWidget::OnBuildingEnd);
		}
		GetBuildingProgress()->StartBuilding();
	}
	if (buildingStateWidget && !GetBuildingProgress()->IsBuildingEnd())
	{
		buildingStateWidget->AddToViewport();
	}
	GetBuildingProgress()->ResumeBuilding();
}

void ABuildingActor::OnEndDetected_Implementation(APlayerController* pPlayer)
{
	if (buildingStateWidget)
	{
		buildingStateWidget->RemoveFromParent();
	}
}
