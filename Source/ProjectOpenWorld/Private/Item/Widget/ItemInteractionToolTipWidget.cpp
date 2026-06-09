#include "Item/Widget/ItemInteractionToolTipWidget.h"
#include "Components/TextBlock.h"
#include "GameBase/FunctionLib/StringTableFunctionLibrary.h"

bool UItemInteractionToolTipWidget::Initialize()
{
	SetVisibility(ESlateVisibility::Hidden);
	return UUserWidget::Initialize();
}

void UItemInteractionToolTipWidget::SetItemName(FName Name)
{
	if (!ItemNameTextBlock)
		return;
	FText ItemName = FText::FromString(Name.ToString());
	ItemName = UStringTableFunctionLibrary::GetItemNameFromStringTable(*ItemName.ToString());
	ItemNameTextBlock->SetText(ItemName);
}
