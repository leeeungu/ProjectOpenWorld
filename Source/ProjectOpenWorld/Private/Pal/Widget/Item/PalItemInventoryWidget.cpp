#include "Pal/Widget/Item/PalItemInventoryWidget.h"
#include "Components/UniformGridPanel.h"
#include "Pal/Widget/Item/PalItemGridSlot.h"

void UPalItemInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UPalItemInventoryWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (inventoryGridPanel && inventoryGridPanel->GetChildrenCount() != Row * Col && SlotClass)
	{
		inventoryGridPanel->ClearChildren();
		for (int i = 0; i < Row; i++)
		{
			for (int k = 0; k < Col; k++)
			{
				UPalItemGridSlot* GridSlot = Cast< UPalItemGridSlot>(CreateWidget(this, SlotClass));
				if (GridSlot)
				{
					inventoryGridPanel->AddChildToUniformGrid(GridSlot, i, k);
					GridSlot->InitializeSlot(this);
				}
			}
		}
	}
}

void UPalItemInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	OnWidgetOpen.Broadcast();
}

void UPalItemInventoryWidget::OnUpdateSlot(int32 Index, const FInventorySlot& ItemSlot)
{
	if (!inventoryGridPanel)
		return;
	if(UPalItemGridSlot* GridSlot = Cast<UPalItemGridSlot>(inventoryGridPanel->GetChildAt(Index)))
	{
		GridSlot->UpdateSlot(ItemSlot);
	}
}

void UPalItemInventoryWidget::OnSlotSwapEvent(UWidget* pSrc, UWidget* pDst)
{
	if (!inventoryGridPanel)
		return;
	int32 Src = inventoryGridPanel->GetChildIndex(pSrc);
	int32 Dst = inventoryGridPanel->GetChildIndex(pDst);
	UE_LOG(LogTemp, Warning, TEXT("UPalItemInventoryWidget :: OnSlotSwapEvent %d %d"), Src, Dst);
	OnSlotSwap.Broadcast(Src, Dst);
}