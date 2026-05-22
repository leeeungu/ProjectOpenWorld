#include "Pal/Widget/Item/PalItemGridSlot.h"
#include "Item/DataTable/ItemSlotType.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Pal/Widget/Item/PalItemInventoryWidget.h"

void UPalItemGridSlot::UpdateSlot(const FInventorySlot& ItemSlot)
{
	FName ItemID = ItemSlot.GetItemID();
	UTexture2D* ItemTexture = UItemDataSubsystem::GetPalItemIconTextureByName(ItemID);
	if (ItemTexture)
	{
		if (ItemImage)
		{
			ItemImage->SetBrushFromTexture(ItemTexture);
			ItemImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		if (ItemCount)
		{
			ItemCount->SetText(FText::AsNumber(ItemSlot.GetItemCount()));
			ItemCount->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
	else
	{
		if (ItemImage)
		{
			ItemImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			ItemImage->SetVisibility(ESlateVisibility::Hidden);	
		}
		if (ItemCount)
		{
			ItemCount->SetText(FText::AsNumber(ItemSlot.GetItemCount()));
			ItemCount->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	bHasItem = nullptr != ItemSlot.ItemObject;
}

void UPalItemGridSlot::InitializeSlot(UPalItemInventoryWidget* Widget)
{
	OwnerWidget = Widget;
}

void UPalItemGridSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
}

void UPalItemGridSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
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

	OutOperation= Cast< UDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass()));
	if (OutOperation && bHasItem)
	{
		OutOperation->Payload = this;
		//OutOperation->DefaultDragVisual = GetInventorySlotWidget();
	}
}

bool UPalItemGridSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (OwnerWidget && InOperation && InOperation->Payload)
	{
		if (UPalItemGridSlot* Other = Cast< UPalItemGridSlot>(InOperation->Payload))
		{
			OwnerWidget->OnSlotSwapEvent(this, Other);
		}
	}
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}
