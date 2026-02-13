#include "Player/Widget/PlayerInventoryWidget.h"
#include "GameBase/Subsystem/UIDataGameInstanceSubsystem.h"

bool UPlayerInventoryWidget::SetMainWidget()
{
	AddToViewport();
	UUIDataGameInstanceSubsystem::PlayUIOpenSound();
	return IsInViewport();
}

void UPlayerInventoryWidget::UnSetMainWidget() 
{
	RemoveFromParent();
	UUIDataGameInstanceSubsystem::PlayUICloseSound();
}