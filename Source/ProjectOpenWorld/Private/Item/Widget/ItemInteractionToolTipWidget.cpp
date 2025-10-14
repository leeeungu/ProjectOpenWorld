#include "Item/Widget/ItemInteractionToolTipWidget.h"
#include "Components/TextBlock.h"

bool UItemInteractionToolTipWidget::Initialize()
{
	SetVisibility(ESlateVisibility::Hidden);
	return UUserWidget::Initialize();
}

void UItemInteractionToolTipWidget::SetItemName(FName Name)
{
	ItemNameTextBlock->SetText(FText::FromString(Name.ToString()));
}
