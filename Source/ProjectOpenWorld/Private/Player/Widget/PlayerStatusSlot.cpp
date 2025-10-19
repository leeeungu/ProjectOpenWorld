#include "Player/Widget/PlayerStatusSlot.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UPlayerStatusSlot::NativePreConstruct()
{
	UUserWidget::NativePreConstruct();
	if (StatusNameText)
		StatusNameText->SetText(StatusName);
	if (StatusImage)
	{
		StatusImage->SetBrushFromTexture(StatusTexture);
	}
}

void UPlayerStatusSlot::UpdateStatus()
{
	if (!StatusRef || !StatusText)
		return;

	StatusText->SetText(FText::Format(FText::FromString(TEXT("{0}")), (int)*StatusRef));
}
