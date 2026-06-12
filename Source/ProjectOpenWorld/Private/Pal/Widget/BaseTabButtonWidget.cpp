#include "Pal/Widget/BaseTabButtonWidget.h"
#include "Pal/Interface/TabWidgetInterface.h"
#include "Components/WidgetSwitcher.h"
#include "Components/PanelWidget.h"

#if WITH_EDITOR
void UBaseTabButtonWidget::OnSelectedByDesigner()
{
	Super::OnSelectedByDesigner();
	ClickEvent();
}
#endif
FReply UBaseTabButtonWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if(ClickEvent())
			return FReply::Handled();
	}
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

bool UBaseTabButtonWidget::ClickEvent()
{
	//if (!GetOuter())
	//	return false;
	//ITabWidgetInterface* SwicherInterface = Cast<ITabWidgetInterface>(GetOuter()->GetOuter());
	//if (Slot && Slot->Parent && SwicherInterface)
	//{
	//	int32 Index = Slot->Parent->GetChildIndex(this);
	//	SwicherInterface->SetSelectedPanel(Index);
	//	return true;
	//}
	//return false;
	const int32 Index = ResolveTabIndex();
	if (Index == INDEX_NONE)
	{
		return false;
	}
	UPanelWidget* TabWidget = GetParent();
	ITabWidgetInterface* TabHost = nullptr;
	while (TabWidget && !TabHost)
	{
		TabHost = Cast<ITabWidgetInterface>(TabWidget);
		TabWidget = TabWidget->GetParent();
	}

	if (TabHost && Index != INDEX_NONE)
	{
		TabHost->SetSelectedPanel(Index);
		return true;
	}
	return false;
}

int32 UBaseTabButtonWidget::ResolveTabIndex() const
{
	if (TabIndex != INDEX_NONE)
	{
		return TabIndex;
	}
	const UPanelWidget* ParentPanel = GetParent();
	return ParentPanel ? ParentPanel->GetChildIndex(this) : INDEX_NONE;
}
