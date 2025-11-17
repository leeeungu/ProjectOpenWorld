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
	GetBuildingProgress()->onBuildingEnd.AddDynamic(this, &ABuildingActor::BuildingEnd);
}

void ABuildingActor::BuildingEnd()
{
	if (Player )
	{
		ABasePlayer* PlayerCharacter = Cast<ABasePlayer>(Player->GetPawn());
		if (PlayerCharacter)
		{
			PlayerCharacter->GetPlayerAnimationComponent()->ResetAnimationState();
		}
	}
}

void ABuildingActor::OnBeginDetected_Implementation(APlayerController* pPlayer)
{
	if (!pPlayer || !pPlayer->GetLocalPlayer())
		return;
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
	//GetBuildingProgress()->StopBuilding();
}

void ABuildingActor::OnInteractionStart_Implementation(APlayerController* pPlayer)
{
	GetBuildingProgress()->StartBuilding();
	Player = pPlayer;
	if (!Player)
		return;

	if (!GetBuildingProgress()->IsBuildingEnd())
	{
		ABasePlayer* PlayerCharacter = Cast<ABasePlayer>(Player->GetPawn());
		if (PlayerCharacter)
		{
			PlayerCharacter->GetPlayerAnimationComponent()->StartArchitecture();
		}
	}
	else if (UBaseBuildingAction* Action = Cast<UBaseBuildingAction>(BuildActionWidget->GetWidget()))
	{
		Action->BuildingAction();
	}

}

void ABuildingActor::OnInteraction_Implementation()
{
	if (!GetBuildingProgress()->IsBuildingEnd())
	{
		GetBuildingProgress()->ResumeBuilding();
	}
}

void ABuildingActor::OnInteractionEnd_Implementation(APlayerController* pPlayer)
{
	
	GetBuildingProgress()->StopBuilding();
	BuildingEnd();
	Player = nullptr;
}
