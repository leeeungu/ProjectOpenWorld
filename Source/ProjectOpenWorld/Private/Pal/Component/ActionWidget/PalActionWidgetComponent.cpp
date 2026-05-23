#include "Pal/Component/ActionWidget/PalActionWidgetComponent.h"
#include "Blueprint/UserWidget.h"

void UPalActionWidgetComponent::CreateActionWidget()
{
	if (ActionWidgetClass)
	{
		ActionWidget = CreateWidget<UUserWidget>(GetWorld(), ActionWidgetClass);
	}
}

void UPalActionWidgetComponent::DeleteActionWidget()
{
	if (ActionWidget)
	{
		ActionWidget->RemoveFromParent();
	}
	ActionWidget = nullptr;
}
