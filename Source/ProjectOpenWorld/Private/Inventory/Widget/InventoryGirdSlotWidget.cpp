#include "Inventory/Widget/InventoryGirdSlotWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Inventory/Widget/InventorySlotWidget.h"
#include "Inventory/Component/InventoryComponent.h"
#include "Item/DataAsset/ItemPrimaryDataAsset.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Inventory/Widget/InventoryDDO.h"

UInventoryGirdSlotWidget::UInventoryGirdSlotWidget(const FObjectInitializer& ObjectInitializer) :
	UUserWidget{ ObjectInitializer }
{
}

void UInventoryGirdSlotWidget::SetSlotData_Implementation(const FInventorySlot& Data)
{
	bool bChange = inventorySlotUW.IsNull();
	itemPointer = &Data;
	if (itemPointer && itemPointer->isEmpthySlot)
	{
		itemPointer = nullptr;
	}
	if (inventorySlotUW)
	{
		IInventorySlotInterface::Execute_SetSlotData(inventorySlotUW.Get(), Data);
	}
}

void UInventoryGirdSlotWidget::SetSlotIndex_Implementation(int Row, int Col)
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

bool UInventoryGirdSlotWidget::SwapSlot_Index(int Row, int Col)
{
	UInventoryComponent* inventoryComponent = GetOwningPlayer()->GetComponentByClass<UInventoryComponent>();
	if (!inventoryComponent)
		return false;
	return inventoryComponent->SwapSlot(inventoryRow, inventoryCol, Row, Col);
}

UInventorySlotWidget* UInventoryGirdSlotWidget::GetInventorySlotWidget() const
{
	return inventorySlotUW.Get();
}

void UInventoryGirdSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (inventorySlotCanvas && inventorySlotCanvas->Slot && inventorySlotCanvas->Slot->Content)
	{
		inventorySlotUW =  Cast<UInventorySlotWidget>(inventorySlotCanvas->GetChildAt(0)) ;
	}
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
	
	UInventoryDDO* DDO = Cast< UInventoryDDO>(UWidgetBlueprintLibrary::CreateDragDropOperation(UInventoryDDO::StaticClass()));
	if (DDO && itemPointer)
	{
		DDO->Payload = this;
		DDO->DefaultDragVisual = GetInventorySlotWidget();
		IInventorySlotInterface::Execute_SetSlotData(DDO, *itemPointer);
		IInventorySlotInterface::Execute_SetSlotIndex(DDO, inventoryRow, inventoryCol);
	}
	OutOperation = DDO;
}

bool UInventoryGirdSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (UInventoryDDO* DDO = Cast< UInventoryDDO>(InOperation))
	{
		return SwapSlot_Index(DDO->GetSlotRow(), DDO->GetSlotCol());
	}
	return UUserWidget::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);;
}

