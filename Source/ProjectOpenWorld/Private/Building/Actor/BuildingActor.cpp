#include "Building/Actor/BuildingActor.h"
#include "Building/Component/BuildingProgress.h"
#include "Building/Subsystem/BuildingWidgetSubsystem.h"
#include "Player/Character/BasePlayer.h"
#include "Player/Component/PlayerAnimationComponent.h"
#include "Building/Component/BuildingActionWidgetComponent.h"
#include "Building/Component/PalBuildingStaticMeshComponent.h"
#include "Building/Widget/BaseBuildingAction.h"
#include "GameBase/Subsystem/UIDataGameInstanceSubsystem.h"

void ABuildingActor::BeginPlay()
{
	Super::BeginPlay();
}

void ABuildingActor::PostLoad()
{
	Super::PostLoad();
	if (BuildingID == "Bp_BuildingActorBase" || BuildingID == NAME_None)
	{
		FString LoadContext = GetClass()->GetName();
		{
			LoadContext.Split(TEXT("_C"), &LoadContext, nullptr);
			LoadContext.Split(TEXT("Building_"), nullptr, &LoadContext);
		}
		BuildingID = *LoadContext;
	}
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
	else if (ABasePlayer* pPlayer = Cast<ABasePlayer>(pOther))
	{
		if (!pPlayer->AddToViewPort(BuildActionWidget->GetWidget()))
		{
			pPlayer->RemoveFromViewPort(BuildActionWidget->GetWidget());
		}
	}
	//else if (UBaseBuildingAction* Action = Cast<UBaseBuildingAction>(BuildActionWidget->GetWidget()))
	//{
	//	if (!Action->IsInViewport())
	//	{
	//		Action->OpenBuildingActionWidget();
	//	}
	//	else
	//	{
	//		Action->CloseBuildingActionWidget();
	//	}
	//}

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
	if (UBuildingWidgetSubsystem* BuildingWidgetSubsystem = pPlayer->GetLocalPlayer()->GetSubsystem<UBuildingWidgetSubsystem>()) // GetSubsystem°¡ Map¿¡¼­ Ã£À¸´Ï ±¦ÂúÀº µí
	{
		BuildingWidgetSubsystem->SetBuildingWidgetProperty(nullptr);
		BuildingWidgetSubsystem->RemoveBuildTimeWidget();
	}
	PalBuildingStaticMeshComponent->StopAll();
	UUIDataGameInstanceSubsystem::PlayUIBuildCancelSound();
	if (GetBuildingProgress())
	{
		UE_LOG(LogTemp, Error, TEXT("ABaseBuilding::BeginDestroy "));
		//GetBuildingProgress()->onBuildingEnd.Broadcast();
		GetBuildingProgress()->EndBuilding();
	}
	Destroy();
}
