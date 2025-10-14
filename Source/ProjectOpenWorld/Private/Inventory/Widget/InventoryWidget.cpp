#include "Inventory/Widget/InventoryWidget.h"
#include "Inventory/Widget/InventorySlotWidget.h"
#include "Components/UniformGridPanel.h"
#include "Inventory/Component/InventoryComponent.h"
#include "GameFramework/PlayerController.h"

void UInventoryWidget::NativeOnInitialized()
{
	UUserWidget::NativeOnInitialized();
	/*inventoryComponent = GetOwningPlayer()->GetComponentByClass<UInventoryComponent>();
	if (!inventoryGridPanel || !inventorySlotClass || !inventoryComponent)
		return;
	inventoryGridPanel->ClearChildren();
	for (int i = 0; i < inventoryComponent->GetInventoryRow(); i++)
	{
		for (int k = 0; k < inventoryComponent->GetInventoryCol(); k++)
		{
			UInventorySlotWidget* InventorySlot = Cast< UInventorySlotWidget>(CreateWidget(this, inventorySlotClass));
			if (InventorySlot)
			{
				inventoryGridPanel->AddChildToUniformGrid(InventorySlot, i, k);
			}
		}
	}*/
}

void UInventoryWidget::NativePreConstruct()
{
	UUserWidget::NativePreConstruct();
	if (!inventoryGridPanel || !inventorySlotClass)
		return;
	inventoryGridPanel->ClearChildren();
	if (!inventoryComponent )
	{
		for (int i = 0; i < 7; i++)
		{
			for (int k = 0; k < 6; k++)
			{
				UInventorySlotWidget* InventorySlot = Cast< UInventorySlotWidget>(CreateWidget(this, inventorySlotClass));
				if (InventorySlot)
				{
					inventoryGridPanel->AddChildToUniformGrid(InventorySlot, i, k);
				}
			}
		}
		return;
	}
	
}

void UInventoryWidget::NativeConstruct()
{
	UUserWidget::NativeConstruct();
	if (!inventoryComponent)
	{
		inventoryComponent = GetOwningPlayer()->GetComponentByClass<UInventoryComponent>();
		if (!inventoryGridPanel || !inventorySlotClass || !inventoryComponent)
			return;
		UE_LOG(LogTemp, Warning, TEXT("Create"));
		inventoryGridPanel->ClearChildren();
		for (int i = 0; i < inventoryComponent->GetInventoryRow(); i++)
		{
			for (int k = 0; k < inventoryComponent->GetInventoryCol(); k++)
			{
				UInventorySlotWidget* InventorySlot = Cast< UInventorySlotWidget>(CreateWidget(this, inventorySlotClass));
				if (InventorySlot)
				{
					inventoryGridPanel->AddChildToUniformGrid(InventorySlot, i, k);
				}
			}
		}
	}
	for (int i = 0; inventoryComponent && i < inventoryComponent->GetInventoryRow(); i++)
	{
		for (int k = 0; k < inventoryComponent->GetInventoryCol(); k++)
		{
			UpdateInventorySlot(i, k);
		}
	}
}

void UInventoryWidget::UpdateInventorySlot(int Row, int Col)
{
	if (!inventoryComponent)
		return;
	FInventorySlot* Data{};
	if (inventoryComponent->GetInventorySlotData(Row, Col, Data))
	{
		UInventorySlotWidget* SlotData = Cast< UInventorySlotWidget>(inventoryGridPanel->GetChildAt(Row* inventoryComponent ->GetInventoryRow() + Col));
		if (SlotData)
		{
			IInventorySlotInterface::Execute_SetSlotData(SlotData, *Data);
		}
	}
}
