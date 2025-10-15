#include "Inventory/Widget/InventoryGirdSlotWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Inventory/Widget/InventorySlotWidget.h"
#include "Inventory/Component/InventoryComponent.h"
#include "Item/DataAsset/ItemPrimaryDataAsset.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

UInventoryGirdSlotWidget::UInventoryGirdSlotWidget(const FObjectInitializer& ObjectInitializer) :
	UUserWidget{ ObjectInitializer }
{
	static ConstructorHelpers::FClassFinder<UUserWidget> DefaultSlot(TEXT("/Game/Inventory/Widget/Slot/WBP_Inventory_Default.WBP_Inventory_Default_C"));
	if (DefaultSlot.Succeeded())
	{
		inventorySlotClass = DefaultSlot.Class;
	}
}

void UInventoryGirdSlotWidget::SetSlotData_Implementation(const FInventorySlot& Data)
{
	bool bChange = inventorySlotUW.IsNull();
	itemPointer = &Data;
	if (itemPointer && itemPointer->isEmpthySlot)
	{
		itemPointer = nullptr;
	}

	TSubclassOf<UInventorySlotWidget> SlotClass = inventorySlotClass;

	if (Data.ItemDataAsset)
		SlotClass = Data.ItemDataAsset->GetInventorySlotWidgetClass();
	if(inventorySlotUW && inventorySlotUW->GetClass() != SlotClass)
	{
		bChange = true;
		if (inventorySlotUW)
			inventorySlotUW->RemoveFromParent();
		inventorySlotUW = nullptr;
	}
	if (bChange)
	{
		inventorySlotUW = CreateWidget(this, SlotClass);
		if (inventorySlotUW && bChange)
		{
			UCanvasPanelSlot* PanelSlot = inventorySlotCanvas->AddChildToCanvas(inventorySlotUW.Get());
			PanelSlot->SetAnchors(FAnchors(0, 0, 1, 1));
			PanelSlot->SetSize(FVector2D::ZeroVector);
		}
	}
	if (inventorySlotUW)
	{
		IInventorySlotInterface::Execute_SetSlotData(inventorySlotUW.Get(), Data);
	}
}

void UInventoryGirdSlotWidget::Init_RowCol(int Row, int Col)
{
	inventoryRow = Row;
	inventoryCol = Col;
}

bool UInventoryGirdSlotWidget::SwapSlot(UInventoryGirdSlotWidget* OtherSlot)
{
	UInventoryComponent * inventoryComponent = GetOwningPlayer()->GetComponentByClass<UInventoryComponent>();
	if (!inventoryComponent || !OtherSlot)
		return false;
	return inventoryComponent->SwapSlot(inventoryRow, inventoryCol, OtherSlot->inventoryRow, OtherSlot->inventoryCol);
}

UInventorySlotWidget* UInventoryGirdSlotWidget::GetInventorySlotWidget() const
{
	return inventorySlotUW.Get();
}

FReply UInventoryGirdSlotWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	UUserWidget::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	return FReply::Handled();
}

FReply UInventoryGirdSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	UUserWidget::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) && itemPointer)
	{
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}
	return FReply::Unhandled();
}

void UInventoryGirdSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	UUserWidget::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
}

bool UInventoryGirdSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (UInventoryGirdSlotWidget* GridSlot = Cast< UInventoryGirdSlotWidget>(InOperation->Payload))
	{
		return SwapSlot(GridSlot);
	}
	return UUserWidget::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);;
}

