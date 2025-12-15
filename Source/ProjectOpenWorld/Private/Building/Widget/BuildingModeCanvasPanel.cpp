#include "Building/Widget/BuildingModeCanvasPanel.h"
#include "Building/Widget/BuildingModeImage.h"
#include "Components/CanvasPanelSlot.h"

void UBuildingModeCanvasPanel::PreConstruct()
{
	if (SlotMaxIndex < 0)
		SlotMaxIndex = GetChildrenCount();
	if (SlotMaxIndex <= 1)
		return;
	int n{};
	double Start = static_cast<double>(180.0 / SlotMaxIndex);
	double degree{};
	if (bCenterStart)
		degree = Start;
	double Radian = FMath::DegreesToRadians(degree);

	if (UPanelSlot* RootPanel = Cast<UPanelSlot>(Slot))
	{
	
		for (int32 i = 0; i < GetChildrenCount(); i++)
		{
			if (UBuildingModeImage* btn = Cast<UBuildingModeImage>(GetChildAt(i)))
			{
				if (UCanvasPanelSlot* PanelSlot = Cast< UCanvasPanelSlot>(btn->Slot))
				{
					PanelSlot->SetAlignment(FVector2D(0.5, 0.5));
					FAnchors Anchor{};
					Anchor.Minimum = FVector2D(0.5, 0.5);
					Anchor.Maximum = FVector2D(0.5, 0.5);
					PanelSlot->SetAnchors(Anchor);
					PanelSlot->SetPosition(FVector2D(FMath::Sin(Radian) * Radius, -FMath::Cos(Radian) * Radius) );
					PanelSlot->SetSize(ButtonSize);
					btn->SetSlotIndex(n);
					n++;
					n %= SlotMaxIndex;
					degree = 0.0;
					if (bCenterStart)
						degree += Start;
					degree += n * static_cast<float>(360 / SlotMaxIndex);
					Radian = FMath::DegreesToRadians(degree);
				}
			}
		}
	}
}
