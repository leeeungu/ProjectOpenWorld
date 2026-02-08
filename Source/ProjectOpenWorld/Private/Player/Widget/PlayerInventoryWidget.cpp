#include "Player/Widget/PlayerInventoryWidget.h"

bool UPlayerInventoryWidget::SetMainWidget()
{
	AddToViewport();
	return IsInViewport();
}

void UPlayerInventoryWidget::UnSetMainWidget() 
{
	RemoveFromParent();
}