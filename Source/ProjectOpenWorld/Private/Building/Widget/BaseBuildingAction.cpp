#include "Building/Widget/BaseBuildingAction.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GameBase/Subsystem/UIDataGameInstanceSubsystem.h"

UBaseBuildingAction::UBaseBuildingAction(const FObjectInitializer& ObjectInitializer) 
	:Super(ObjectInitializer)
{
}

void UBaseBuildingAction::NativeConstruct()
{
	UUserWidget::NativeConstruct();
}

void UBaseBuildingAction::NativeDestruct()
{
	UUserWidget::NativeDestruct();
}

bool UBaseBuildingAction::SetMainWidget()
{
	APlayerController* pc = GetOwningPlayer();
	if (!pc || IsInViewport())
		return false;
	AddToViewport();
	UUIDataGameInstanceSubsystem::PlayUIOpenSound();
	pc->SetShowMouseCursor(true);
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(pc, this, EMouseLockMode::DoNotLock, true, true);
	UGameplayStatics::SetViewportMouseCaptureMode(pc->GetWorld(), EMouseCaptureMode::NoCapture);
	return true;
}

void UBaseBuildingAction::UnSetMainWidget()
{
	APlayerController* pc = GetOwningPlayer();
	if (!pc || !IsInViewport())
		return;
	RemoveFromParent();
	UUIDataGameInstanceSubsystem::PlayUICloseSound();
	pc->SetShowMouseCursor(false);
	UGameplayStatics::SetViewportMouseCaptureMode(pc->GetWorld(), EMouseCaptureMode::CaptureDuringMouseDown);
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(pc, true);
	UWidgetBlueprintLibrary::CancelDragDrop();
}
