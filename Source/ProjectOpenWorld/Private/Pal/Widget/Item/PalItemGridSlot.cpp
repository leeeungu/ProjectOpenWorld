#include "Pal/Widget/Item/PalItemGridSlot.h"
#include "Item/DataTable/ItemSlotType.h"
#include "Components/Image.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Pal/Widget/Item/PalItemInventoryWidget.h"
#include "Inventory/Widget/InventorySlotWidget.h"
#include "GameBase/Subsystem/UIDataGameInstanceSubsystem.h"
#include "Inventory/Widget/InventorySlotBase.h"
#include "Inventory/Component/InventoryComponent.h"

void UPalItemGridSlot::UpdateSlot(const FInventorySlot& ItemSlot)
{
	ItemObj = ItemSlot.ItemObject;
	if (InventorySlotWidget)
	{
		IInventorySlotInterface::Execute_SetSlotData(InventorySlotWidget.Get(), ItemSlot);
	}
	bHasItem = nullptr != ItemSlot.ItemObject;
}

void UPalItemGridSlot::InitializeSlot(UPalItemInventoryWidget* Widget)
{
	OwnerWidget = Widget;
}

bool UPalItemGridSlot::AddItemToInventory(const FInventorySlot& ItemSlot)
{
	return false;
}

void UPalItemGridSlot::OnSlotRemoveEvent(UDragDropOperation* Operation)
{
	if (Operation && OwnerWidget && !Cast< UPalItemGridSlot>(Operation->Payload))
	{
		OwnerWidget->OnSlotRemoveEvent(this);
	}
}

void UPalItemGridSlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UPalItemGridSlot::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UPalItemGridSlot::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPalItemGridSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (IsHovered())
	{
		UUIDataGameInstanceSubsystem::PlayUIHoverSound();
		SlotFrame->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UPalItemGridSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	SlotFrame->SetVisibility(ESlateVisibility::Hidden);
}

FReply UPalItemGridSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) && bHasItem)
	{
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UPalItemGridSlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	OutOperation = UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass());
	if (OutOperation && bHasItem)
	{
		OutOperation->Payload = this;
		OutOperation->DefaultDragVisual = InventorySlotWidget;
		OutOperation->OnDrop.AddUniqueDynamic(this, &UPalItemGridSlot::OnSlotRemoveEvent);
	}
}

bool UPalItemGridSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (OwnerWidget && InOperation && InOperation->Payload)
	{
		UPalItemGridSlot* OtherGrid = Cast< UPalItemGridSlot>(InOperation->Payload);
		if (OtherGrid)
		{
			OwnerWidget->OnSlotSwapEvent(this, OtherGrid);
		}
		UInventorySlotBase* Other = Cast< UInventorySlotBase>(InOperation->Payload);
		if (Other && Other->GetSlotDataPtr() && Other->GetSlotDataPtr()->ItemObject)
		{
			if (OwnerWidget->OnAddItemEvent(Other->GetSlotDataPtr()->ItemObject))
			{
				InOperation->Payload = this;
				return true;
			}
		}
	}
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}
