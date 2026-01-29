#include "Building/Widget/BaseBuildingAction.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

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

void UBaseBuildingAction::OpenBuildingActionWidget()
{
	APlayerController* pc = GetOwningPlayer();
	if (!pc || IsInViewport())
		return;
	AddToViewport();
	pc->SetShowMouseCursor(true);
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(pc, this, EMouseLockMode::DoNotLock, true, true);
	UGameplayStatics::SetViewportMouseCaptureMode(pc->GetWorld(), EMouseCaptureMode::NoCapture);
}

void UBaseBuildingAction::CloseBuildingActionWidget()
{
	APlayerController* pc = GetOwningPlayer();
	if (!pc || !IsInViewport())
		return;
	RemoveFromParent();
	pc->SetShowMouseCursor(false);
	UGameplayStatics::SetViewportMouseCaptureMode(pc->GetWorld(), EMouseCaptureMode::CaptureDuringMouseDown);
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(pc, true);
	UWidgetBlueprintLibrary::CancelDragDrop();
}

void UBaseBuildingAction::BuildingAction()
{
	APlayerController* pc = GetOwningPlayer();
	if (!pc)
	{
		RemoveFromParent();
		return;
	}
	if (pc->bShowMouseCursor)
	{
		CloseBuildingActionWidget();
	}
	else
	{
		OpenBuildingActionWidget();
	}

}
