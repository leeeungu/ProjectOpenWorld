#include "Pal/Widget/PalBox/PalInventoryWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/TextBlock.h"
#include "Pal/Widget/PalBox/PalInventorySlot.h"
#include "Creature/Character/BaseCreature.h"
#include "Pal/Widget/PalBox/PalBoxWidget.h"

void UPalInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (!PalBoxGridPanel || !InventorySlotClass)
		return;
	PalBoxGridPanel->ClearChildren();
	for (int32 IndexRow = 0; IndexRow < NumRows; ++IndexRow)
	{
		for (int32 IndexCol = 0; IndexCol < NumColumns; ++IndexCol)
		{
			UPalInventorySlot* NewSlotWidget = CreateWidget<UPalInventorySlot>(this, InventorySlotClass);
			if (NewSlotWidget)
			{
				PalBoxGridPanel->AddChildToUniformGrid(NewSlotWidget, IndexRow, IndexCol);
			}
		}
	}
}

void UPalInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPalInventoryWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	PalBoxGridPanel->ClearChildren();
	for (int32 IndexRow = 0; IndexRow < NumRows; ++IndexRow)
	{
		for (int32 IndexCol = 0; IndexCol < NumColumns; ++IndexCol)
		{
			UPalInventorySlot* NewSlotWidget = CreateWidget<UPalInventorySlot>(this, InventorySlotClass);
			if (NewSlotWidget)
			{
				PalBoxGridPanel->AddChildToUniformGrid(NewSlotWidget, IndexRow, IndexCol);
			}
		}
	}
}

void UPalInventoryWidget::SetPalSlot()
{
	UObject* OuterWidget = GetOuter();
	UPalBoxWidget* ParentWidget{};
	while (OuterWidget && !ParentWidget)
	{
		ParentWidget = Cast<UPalBoxWidget>(OuterWidget);
		OuterWidget = OuterWidget->GetOuter();
	}
	TArray<TObjectPtr<ABaseCreature>> SelectedPals;
	if (ParentWidget)
	{
		ParentWidget->GetSelectedPalArray(SelectedPals);
	}

	for (int32 IndexRow = 0; IndexRow < NumRows; ++IndexRow)
	{
		for (int32 IndexCol = 0; IndexCol < NumColumns; ++IndexCol)
		{
			int32 SlotIndex = IndexRow * NumColumns + IndexCol;
			if (UPalInventorySlot* SlotWidget = Cast<UPalInventorySlot>(PalBoxGridPanel->GetChildAt(SlotIndex)))
			{
				ABaseCreature* SelectedPal = nullptr;
				if (SelectedPals.IsValidIndex(SlotIndex))
				{
					SelectedPal = SelectedPals[SlotIndex];
				}
				SlotWidget->SetPalCreature(SelectedPal);
			}
		}
	}
}
