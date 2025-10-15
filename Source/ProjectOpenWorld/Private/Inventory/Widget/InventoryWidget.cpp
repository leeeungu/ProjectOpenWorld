#include "Inventory/Widget/InventoryWidget.h"
#include "Inventory/Widget/InventorySlotWidget.h"
#include "Inventory/Widget/InventoryGirdSlotWidget.h"
#include "Components/UniformGridPanel.h"
#include "Inventory/Component/InventoryComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UInventoryWidget::NativeOnInitialized()
{
	UUserWidget::NativeOnInitialized();
}

void UInventoryWidget::NativePreConstruct()
{
	UUserWidget::NativePreConstruct();
	if (!inventoryGridPanel || !inventorySlotClass || !GetOwningPlayer())
		return;
	inventoryComponent = GetOwningPlayer()->GetComponentByClass<UInventoryComponent>();
	if (!inventoryComponent)
	{
		inventoryGridPanel->ClearChildren();
		UpdateInventoryWeight();
		for (int i = 0; i < 7; i++)
		{
			for (int k = 0; k < 6; k++)
			{
				UInventoryGirdSlotWidget* InventorySlot = Cast< UInventoryGirdSlotWidget>(CreateWidget(this, inventorySlotClass));
				if (InventorySlot)
				{
					inventoryGridPanel->AddChildToUniformGrid(InventorySlot, i, k);
					InventorySlot->Init_RowCol(i, k);
				}
			}
		}
		return;
	}

	inventoryComponent->onUpdateInventory.RemoveDynamic(this, &UInventoryWidget::UpdateAllInventorySlot);
	inventoryComponent->onUpdateInventory.AddDynamic(this, &UInventoryWidget::UpdateAllInventorySlot);
	inventoryGridPanel->ClearChildren();
	for (int i = 0; i < inventoryComponent->GetInventoryRow(); i++)
	{
		for (int k = 0; k < inventoryComponent->GetInventoryCol(); k++)
		{
			UInventoryGirdSlotWidget* InventorySlot = Cast< UInventoryGirdSlotWidget>(CreateWidget(this, inventorySlotClass));
			if (InventorySlot)
			{
				inventoryGridPanel->AddChildToUniformGrid(InventorySlot, i, k);
				InventorySlot->Init_RowCol(i, k);
			}
		}
	}
	UpdateInventoryWeight();
}

void UInventoryWidget::NativeConstruct()
{
	UUserWidget::NativeConstruct();
	UpdateAllInventorySlot();
}

void UInventoryWidget::UpdateInventorySlot(int Row, int Col)
{
	if (!inventoryComponent)
		return;
	const FInventorySlot* Data{};
	if (inventoryComponent->GetInventorySlotData(Row, Col, Data))
	{
		TScriptInterface<IInventorySlotInterface> SlotData = TScriptInterface< IInventorySlotInterface>(inventoryGridPanel->GetChildAt(Row* inventoryComponent ->GetInventoryCol() + Col));
		if (SlotData && Data)
		{
			IInventorySlotInterface::Execute_SetSlotData(SlotData.GetObject(), *Data);
		}
	}
}

void UInventoryWidget::UpdateAllInventorySlot()
{
	if (!inventoryComponent)
		return;
	for (int i = 0; i < inventoryComponent->GetInventoryRow(); i++)
	{
		for (int k = 0; k < inventoryComponent->GetInventoryCol(); k++)
		{
			UpdateInventorySlot(i, k);
		}
	}
	UpdateInventoryWeight();
}

void UInventoryWidget::UpdateInventoryWeight()
{
	int MaxWeight = inventoryComponent ? inventoryComponent->GetInventoryMaxWeight() : 0;
	int CurWeight = inventoryComponent ? inventoryComponent->GetInventoryWeight() : 1;
	WeightProgressBar->SetPercent(CurWeight / (float)MaxWeight);
	WeightPercentTextBlock->SetText(FText::Format(FText::FromString(TEXT("{0} / {1}")), CurWeight, MaxWeight));
}
