#include "Building/Actor/BuildingActor.h"
#include "Building/Component/BuildingProgress.h"
#include "Building/Subsystem/BuildingWidgetSubsystem.h"
#include "Player/Character/BasePlayer.h"
#include "Player/Component/PlayerAnimationComponent.h"
#include "Building/Component/BuildingActionWidgetComponent.h"
#include "Building/Widget/BaseBuildingAction.h"

void ABuildingActor::BeginPlay()
{
	Super::BeginPlay();
}

void ABuildingActor::OnBeginDetected_Implementation(APlayerController* pPlayer)
{
	if (!pPlayer || !pPlayer->GetLocalPlayer())
		return;
	Player = pPlayer;
	if (UBuildingWidgetSubsystem* BuildingWidgetSubsystem = pPlayer->GetLocalPlayer()->GetSubsystem<UBuildingWidgetSubsystem>()) // GetSubsystem°¡ Map¿¡¼­ Ã£À¸´Ï ±¦ÂúÀº µí
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
}

void ABuildingActor::OnInteractionStart_Implementation(APlayerController* pPlayer)
{
	if (!pPlayer)
		return;
	if (!GetBuildingProgress()->IsBuildingEnd())
	{
		GetBuildingProgress()->StartBuilding(pPlayer->GetPawn());
	}
	else if (UBaseBuildingAction* Action = Cast<UBaseBuildingAction>(BuildActionWidget->GetWidget()))
	{
		
		Action->BuildingAction();
	}

}

void ABuildingActor::OnInteraction_Implementation(APlayerController* pPlayer)
{
	if (!GetBuildingProgress()->IsBuildingEnd())
	{
	//	GetBuildingProgress()->StopBuilding(pPlayer->GetPawn());
	}
}

void ABuildingActor::OnInteractionEnd_Implementation(APlayerController* pPlayer)
{
	GetBuildingProgress()->StopBuilding(pPlayer->GetPawn());
}

void ABuildingActor::OnInteractionCanceled_Implementation(APlayerController* pPlayer)
{
	if (!Player)
		return;
	if (UBuildingWidgetSubsystem* BuildingWidgetSubsystem = pPlayer->GetLocalPlayer()->GetSubsystem<UBuildingWidgetSubsystem>()) // GetSubsystem°¡ Map¿¡¼­ Ã£À¸´Ï ±¦ÂúÀº µí
	{
		BuildingWidgetSubsystem->SetBuildingWidgetProperty(nullptr);
		BuildingWidgetSubsystem->RemoveBuildTimeWidget();
	}
	buildingProgressComponent->StopAll();
	//if (buildingProgressComponent)
	//{
	//	UE_LOG(LogTemp, Error, TEXT("ABaseBuilding::BeginDestroy "));
	//	//buildingProgressComponent->onBuildingEnd.Broadcast();
	//}
	Destroy();
}
