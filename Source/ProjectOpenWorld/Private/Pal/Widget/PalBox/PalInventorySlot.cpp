#include "Pal/Widget/PalBox/PalInventorySlot.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Creature/Character/BaseCreature.h"
#include "Pal/Subsystem/PalCharacterDataSubsystem.h"
#include "Pal/Widget/PalBox/PalBoxWidget.h"
#include "Blueprint/DragDropOperation.h"

void UPalInventorySlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (SlotButton)
	{
		SlotButton->OnClicked.AddDynamic(this, &UPalInventorySlot::OnSlotButtonClicked);
	}
}

void UPalInventorySlot::SetPalCreature(ABaseCreature* SelectedCreature)
{
	CurrentSelectedCreature = SelectedCreature;
	SlotImage->SetVisibility(ESlateVisibility::Hidden);
	if (CurrentSelectedCreature.IsValid())
	{
		if(SlotImage)
		{
			UTexture2D* IconDataRow = UPalCharacterDataSubsystem::GetPalCharacterIconByName(CurrentSelectedCreature->GetPalID());
			if (IconDataRow)
			{
				SlotImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
			SlotImage->SetBrushFromTexture(IconDataRow);
		}
	}
}

void UPalInventorySlot::OnSlotButtonClicked()
{	
	if (CurrentSelectedCreature.IsValid())
	{
		UObject* OuterWidget = GetOuter();
		UPalBoxWidget* ParentWidget{};
		while (OuterWidget && !ParentWidget)
		{
			ParentWidget = Cast<UPalBoxWidget>(OuterWidget);
			OuterWidget = OuterWidget->GetOuter();
		}

		if (ParentWidget)
		{
			ParentWidget->OnPalSelectedChanged(CurrentSelectedCreature.Get());
		}
	}
	
}

void UPalInventorySlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	if (OutOperation)
	{
		OutOperation->Payload = this;
	}
}

void UPalInventorySlot::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
}

void UPalInventorySlot::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
}

bool UPalInventorySlot::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	return Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);
}

bool UPalInventorySlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (InOperation && InOperation->Payload)
	{
		if (UPalInventorySlot* Other = Cast< UPalInventorySlot>(InOperation->Payload))
		{
			ABaseCreature* TempCreature = Other->CurrentSelectedCreature.Get();
			Other->SetPalCreature(this->CurrentSelectedCreature.Get());
			this->SetPalCreature(TempCreature);
			return true;
		}
	}
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UPalInventorySlot::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
}

//void UPalInventorySlot::OnSlotButtonHovered()
//{
//}
//
//void UPalInventorySlot::OnSlotButtonUnhovered()
//{
//}
