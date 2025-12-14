#include "Building/Widget/BuildingModeWidget.h"
#include "Building/BuildingAssistComponent.h"
#include "Player/Character/BasePlayer.h" 

void UBuildingModeWidget::NativeOnInitialized()
{
	UUserWidget::NativeOnInitialized();
	ABasePlayer* Player = GetOwningPlayer()->GetPawn<ABasePlayer>();
	if (!Player)
		return;
	BuildingAssistComp = Player->GetBuildingAssist();
}

void UBuildingModeWidget::NativePreConstruct()
{
	UUserWidget::NativePreConstruct();
}

void UBuildingModeWidget::NativeConstruct()
{
	UUserWidget::NativeConstruct();
}

void UBuildingModeWidget::NativeDestruct()
{
	UUserWidget::NativeDestruct();
}

void UBuildingModeWidget::StartBuildingMode()
{
	if (!BuildingAssistComp || !CurrentHoverMesh)
		return;
	BuildingAssistComp->SetBuildingStaticMesh(CurrentHoverMesh);
	BuildingAssistComp->StartBuilding();
}

void UBuildingModeWidget::HoverBuildingMode(int SlotIndex, UStaticMesh* Mesh)
{
	CurrentHoverMesh = Mesh;
}

