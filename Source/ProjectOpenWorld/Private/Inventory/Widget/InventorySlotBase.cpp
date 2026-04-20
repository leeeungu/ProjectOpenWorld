#include "Inventory/Widget/InventorySlotBase.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Inventory/Widget/InventorySlotWidget.h"
#include "Inventory/Component/InventoryComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Inventory/Widget/InventoryDDO.h"
#include "Inventory/Widget/InventorySlotToolTip.h"
#include "GameBase/Subsystem/UIDataGameInstanceSubsystem.h"
#include "Components/Image.h"
#include "Player/Controller/BasePlayerController.h"
#include "Item/DataTable/ItemSlotType.h"


UInventorySlotBase::UInventorySlotBase(const FObjectInitializer& ObjectInitializer) :
	UUserWidget{ ObjectInitializer }
{
}

void UInventorySlotBase::SetSlotData_Implementation(const FInventorySlot& Data)
{
}

void UInventorySlotBase::SetSlotPtr(const FInventorySlot* Data)
{
	itemPointer = Data;

	if (!itemPointer)
	{
		itemPointer = nullptr;
	}
	else if (UInventoryComponent* inventoryComponent = GetInventoryComponent())
	{
		int InventoryCol = inventoryComponent->GetInventoryCol();
		inventoryRow = Data->SlotIndex / InventoryCol;
		inventoryCol = Data->SlotIndex % InventoryCol;
	}
	if (inventorySlotUW)
	{
		IInventorySlotInterface::Execute_SetSlotData(inventorySlotUW.Get(), *Data);
	}
	UInventorySlotToolTip* toolTipWidget = Cast<UInventorySlotToolTip>(GetToolTip());
	if (toolTipWidget && Data)
	{
		toolTipWidget->SetRecipeID(Data->GetItemID());
	}
}

void UInventorySlotBase::SetSlotIndex_Implementation(int Row, int Col)
{
}

bool UInventorySlotBase::SwapSlot(UInventorySlotBase* OtherSlot)
{
	if (!OtherSlot)
		return false;
	return SwapSlotPtr(OtherSlot->GetInventorySlotData());
}

bool UInventorySlotBase::SwapSlotPtr(const FInventorySlot* Dst)
{
	UInventoryComponent* inventoryComponent = GetInventoryComponent();
	if (!inventoryComponent || !Dst)
		return false;
	FInventorySlot* pSrc = const_cast<FInventorySlot*>(itemPointer);
	FInventorySlot* pDst = const_cast<FInventorySlot*>(Dst);
	if(!pSrc || !pDst)
		return false;
	return inventoryComponent->SwapInventorySlot(pSrc, pDst);
}

UInventorySlotWidget* UInventorySlotBase::GetInventorySlotWidget() const
{
	return inventorySlotUW.Get();
}

void UInventorySlotBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	SetToolTip(CreateWidget<UInventorySlotToolTip>(this, ToolTipWidgetClass));
}

void UInventorySlotBase::NativeConstruct()
{
	Super::NativeConstruct();
	if (inventorySlotCanvas && inventorySlotCanvas->Slot && inventorySlotCanvas->Slot->Content)
	{
		inventorySlotUW = Cast<UInventorySlotWidget>(inventorySlotCanvas->GetChildAt(0));
	}
}

void UInventorySlotBase::NativeDestruct()
{
	Super::NativeDestruct();
	SlotFrame->SetVisibility(ESlateVisibility::Hidden);
}

void UInventorySlotBase::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	if (IsHovered())
	{
		UUIDataGameInstanceSubsystem::PlayUIHoverSound();
		SlotFrame->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UInventorySlotBase::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	SlotFrame->SetVisibility(ESlateVisibility::Hidden);
}

FReply UInventorySlotBase::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		UUIDataGameInstanceSubsystem::PlayButtonClickSound();
	}
	return Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
}

FReply UInventorySlotBase::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) && itemPointer)
	{
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UInventorySlotBase::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

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

bool UInventorySlotBase::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (UInventoryDDO* DDO = Cast< UInventoryDDO>(InOperation))
	{
		const FInventorySlot* srcSlot= itemPointer;
		bool bIsSameSlot = SwapSlot(Cast< UInventorySlotBase>(DDO->Payload));
		return bIsSameSlot;
	}
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);;
}

UInventoryComponent* UInventorySlotBase::GetInventoryComponent() const
{
	ABasePlayerController* PlayerController = Cast< ABasePlayerController>(GetOwningPlayer());
	if (PlayerController)
	{
		return PlayerController->GetInventoryComponent();
	}
	return nullptr;
}