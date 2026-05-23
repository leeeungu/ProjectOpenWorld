#include "Pal/Widget/PalBox/PalBoxSwitchButton.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UPalBoxSwitchButton::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (ButtonImage)
	{
		if (Image)
		{
			ButtonImage->SetBrushFromTexture(Image);
		}
	}
	if (ButtonNameText)
	{
		ButtonNameText->SetText(NameText);
	}
}
