#include "Pal/Widget/PatternWidget_Anubis.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/SizeBox.h"
#include "Components/Image.h"

void UPatternWidget_Anubis::SetPatternImage(int32 Count)
{
	PatternBox->ClearChildren();
	nCount = 0;
	PatternSizeBox->SetWidthOverride(Count * 100.f);
	for (int i = 0; i < Count; i++)
	{
		UImage* Image = NewObject<UImage>(this);
		if (Image)
		{
			Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			Image->SetBrushFromTexture(PatternImage);
			UHorizontalBoxSlot* BoxSlot = PatternBox->AddChildToHorizontalBox(Image);
			if (BoxSlot)
			{
				BoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
				BoxSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
				BoxSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
			}
		}
	}
}

void UPatternWidget_Anubis::UpdatePatternImage()
{
	if (PatternBox->GetChildrenCount() > nCount)
	{
		if (UImage* Image = Cast<UImage>(PatternBox->GetChildAt(nCount)))
		{
			Image->SetBrushFromTexture(SucceedImage);
		}
		nCount++;
	}
}
