#include "Pal/Widget/SwitcherCanvasPanel.h"
#include "Components/WidgetSwitcher.h"

void USwitcherCanvasPanel::SetSelectedPanel(int32 Index)
{
	if (TargetSwitcher && Index >= 0 && Index < TargetSwitcher->GetChildrenCount())
	{
		DefaultIndex = Index;
		TargetSwitcher->SetActiveWidgetIndex(Index);
	}
}

void USwitcherCanvasPanel::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	SetSelectedPanel(DefaultIndex);
}