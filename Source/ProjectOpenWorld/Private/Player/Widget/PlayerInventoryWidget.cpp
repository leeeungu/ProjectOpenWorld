#include "Player/Widget/PlayerInventoryWidget.h"
#include "GameBase/Subsystem/UIDataGameInstanceSubsystem.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

bool UPlayerInventoryWidget::SetMainWidget()
{
	AddToViewport();
	UUIDataGameInstanceSubsystem::PlayUIOpenSound();
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(GetOwningPlayer(), this, EMouseLockMode::DoNotLock, true, true);
	UGameplayStatics::SetViewportMouseCaptureMode(GetOwningPlayer()->GetWorld(), EMouseCaptureMode::NoCapture);
	return IsInViewport();
}

void UPlayerInventoryWidget::UnSetMainWidget() 
{
	RemoveFromParent();
	UUIDataGameInstanceSubsystem::PlayUICloseSound();
	UGameplayStatics::SetViewportMouseCaptureMode(GetOwningPlayer()->GetWorld(), EMouseCaptureMode::CaptureDuringMouseDown);
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(GetOwningPlayer(), true);
	UWidgetBlueprintLibrary::CancelDragDrop();
}