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
				NewSlotWidget->SetSlotIndex(IndexRow * NumColumns + IndexCol);
				PalBoxGridPanel->AddChildToUniformGrid(NewSlotWidget, IndexRow, IndexCol);
			}
		}
	}
	UObject* OuterWidget = GetOuter();
	while (OuterWidget && !ParentPalBoxWidget)
	{
		ParentPalBoxWidget = Cast<UPalBoxWidget>(OuterWidget);
		OuterWidget = OuterWidget->GetOuter();
	}
}

void UPalInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UObject* OuterWidget = GetOuter();
	while (OuterWidget && !ParentPalBoxWidget)
	{
		ParentPalBoxWidget = Cast<UPalBoxWidget>(OuterWidget);
		OuterWidget = OuterWidget->GetOuter();
	}
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
				NewSlotWidget->SetSlotIndex(IndexRow * NumColumns + IndexCol);
				PalBoxGridPanel->AddChildToUniformGrid(NewSlotWidget, IndexRow, IndexCol);
			}
		}
	}
}

void UPalInventoryWidget::UpdatePalInventory(int InventoryIndex, TObjectPtr<ABaseCreature> NewPal)
{
	int32 IndexRow = InventoryIndex / NumColumns;
	int32 IndexCol = InventoryIndex % NumColumns;

	int32 SlotIndex = IndexRow * NumColumns + IndexCol;
	if (UPalInventorySlot* SlotWidget = Cast<UPalInventorySlot>(PalBoxGridPanel->GetChildAt(SlotIndex)))
	{
		TObjectPtr<ABaseCreature> SelectedPal = nullptr;
		if (ParentPalBoxWidget)
		{
			SelectedPal = ParentPalBoxWidget->GetPalInInventory(SlotIndex);
		}
		SlotWidget->SetPalCreature(SelectedPal);
	}
}

void UPalInventoryWidget::SetPalSlot()
{
	for (int32 IndexRow = 0; IndexRow < NumRows; ++IndexRow)
	{
		for (int32 IndexCol = 0; IndexCol < NumColumns; ++IndexCol)
		{
			int32 SlotIndex = IndexRow * NumColumns + IndexCol;
			if (UPalInventorySlot* SlotWidget = Cast<UPalInventorySlot>(PalBoxGridPanel->GetChildAt(SlotIndex)))
			{
				TObjectPtr<ABaseCreature> SelectedPal = nullptr;
				if (ParentPalBoxWidget)
				{
					SelectedPal = ParentPalBoxWidget->GetPalInInventory(SlotIndex);
				}
				SlotWidget->SetPalCreature(SelectedPal);
			}
		}
	}
}

