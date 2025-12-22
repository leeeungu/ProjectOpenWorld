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

void ABuildingActor::OnBeginDetected_Implementation(ACharacter* pOther)
{
	if (!pOther)
		return;
	APlayerController* pPlayer = Cast< APlayerController>(pOther->GetController());
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

void ABuildingActor::OnEndDetected_Implementation(ACharacter* pOther)
{
	if (!pOther)
		return;
	APlayerController* pPlayer = Cast< APlayerController>(pOther->GetController());
	if (!pPlayer || !pPlayer->GetLocalPlayer())
		return;
	if (UBuildingWidgetSubsystem* BuildingWidgetSubsystem = pPlayer->GetLocalPlayer()->GetSubsystem<UBuildingWidgetSubsystem>())
	{
		BuildingWidgetSubsystem->RemoveBuildTimeWidget();
	}
}

void ABuildingActor::OnInteractionStart_Implementation(ACharacter* pOther)
{
	if (!pOther)
		return;
	if (!GetBuildingProgress()->IsBuildingEnd())
	{
		GetBuildingProgress()->StartBuilding(pOther);
	}
	else if (UBaseBuildingAction* Action = Cast<UBaseBuildingAction>(BuildActionWidget->GetWidget()))
	{
		
		Action->BuildingAction();
	}

}

void ABuildingActor::OnInteraction_Implementation(ACharacter* pOther)
{
	if (!GetBuildingProgress()->IsBuildingEnd())
	{
	//	GetBuildingProgress()->StopBuilding(pPlayer->GetPawn());
	}
}

void ABuildingActor::OnInteractionEnd_Implementation(ACharacter* pOther)
{
	if (!pOther)
		return;
	GetBuildingProgress()->StopBuilding(pOther);
}

void ABuildingActor::OnInteractionCanceled_Implementation(ACharacter* pOther)
{
	if (!pOther)
		return;
	APlayerController* pPlayer = Cast< APlayerController>(pOther->GetController());
	if (!pPlayer)
		return;
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
