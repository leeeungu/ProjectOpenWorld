#include "Building/Widget/WorkBenchActionWidget.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanelSlot.h"
#include "Fonts/SlateFontInfo.h"
#include "Engine/Font.h"

UWorkBenchActionWidget::UWorkBenchActionWidget(const FObjectInitializer& ObjectInitializer) :
	UBaseBuildingAction(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UFont> RobotoFontObj(*UWidget::GetDefaultFontName());
	if (RobotoFontObj.Succeeded() && BenchTitleText)
	{
		BenchTitleText->SetFont(FSlateFontInfo(RobotoFontObj.Object, 30, FName("Bold")));
	}
}

void UWorkBenchActionWidget::NativePreConstruct()
{
	UBaseBuildingAction::NativePreConstruct();
	if (BenchTitleText)
	{
		if (UCanvasPanelSlot* pSlot = Cast< UCanvasPanelSlot>(BenchTitleText->Slot))
		{
			pSlot->SetAnchors(FAnchors(0, 0, 1, 1));
			pSlot->SetPosition({ 0, 10.0f });
		}
		BenchTitleText->SetJustification(ETextJustify::Type::Left);
		BenchTitleText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}
