#include "Building/Widget/BuildingSwtichWidget.h"
#include "Components/PanelSlot.h"
#include "Components/PanelWidget.h"
#include "Building/Widget/BuildingModeWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "GameBase/Subsystem/UIDataGameInstanceSubsystem.h"

void UBuildingSwtichWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (GetOuter())
	{
		ParentWidget = Cast<UBuildingModeWidget>(GetOuter()->GetOuter());
	}

	if (SwtichName)
	{
		SwtichName->SetText(SwitchDisplayName);
	}
	if (SwtichImage && SwitchDisplayImage)
	{
		SwtichImage->SetBrushFromTexture(SwitchDisplayImage);
	}
}

void UBuildingSwtichWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (SwtichButton)
	{
		SwtichButton->OnClicked.AddUniqueDynamic(this, &UBuildingSwtichWidget::OnSwtichClicked);
		SwtichButton->OnHovered.AddUniqueDynamic(this, &UBuildingSwtichWidget::OnSwtichHovered);
	}
}

void UBuildingSwtichWidget::OnSwtichClicked()
{
	if (ParentWidget)
	{
		ParentWidget->SetBuildingUI(BuildingIDs);
		UUIDataGameInstanceSubsystem::PlayUITabChangeSound();
	}
}

void UBuildingSwtichWidget::OnSwtichHovered()
{
//	UUIDataGameInstanceSubsystem::PlayUIBuildHoverSound();
}
