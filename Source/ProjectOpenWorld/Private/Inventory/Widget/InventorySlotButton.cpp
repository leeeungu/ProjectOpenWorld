#include "Inventory/Widget/InventorySlotButton.h"
#include "Inventory/Widget/InventoryDDO.h"
#include "GameBase/Widget/ConfirmationWidgetBase.h"
#include "GameBase/Subsystem/UIDataGameInstanceSubsystem.h"

UInventorySlotButton::UInventorySlotButton(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	///Script/UMGEditor.WidgetBlueprint'/Game/Widget/WBP_Confirmationwindow.WBP_Confirmationwindow'
	static ConstructorHelpers::FClassFinder<UUserWidget> UWClass(TEXT("/Game/Widget/WBP_Confirmationwindow.WBP_Confirmationwindow_C"));
	if (UWClass.Succeeded())
	{
		ConfrirmationWidget = UWClass.Class;
	}
}

void UInventorySlotButton::SetSlotData_Implementation(const FInventorySlot& Data)
{
	SlotData = &Data;
}

void UInventorySlotButton::SetSlotIndex_Implementation(int Row, int Col)
{
	SlotRow = Row;
	SlotCol = Col;
}

void UInventorySlotButton::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	UUIDataGameInstanceSubsystem::PlayUIHoverSound();
}

bool UInventorySlotButton::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool Result = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	if (UInventoryDDO* DDO = Cast< UInventoryDDO>(InOperation))
	{
		IInventorySlotInterface::Execute_SetSlotData(this, DDO->GetSlotData());
		IInventorySlotInterface::Execute_SetSlotIndex(this, DDO->GetSlotRow(), DDO->GetSlotCol());
		UConfirmationWidgetBase* Widget = Cast< UConfirmationWidgetBase>(CreateWidget(GetWorld(), ConfrirmationWidget));
		if (Widget)
		{
			Widget->SetEventInterface(this);
			Widget->SetAlertMessage(AlertMessage);
			Widget->AddToViewport();
			Widget->SetFocus();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UInventorySlotButton::NativeOnDrop - ConfrirmationWidgetclass Cnat Find"));
		}
		return true;
	}
	IConfirmationInterface::Execute_ConfirmationEvent(this, false);
	return Result;
}