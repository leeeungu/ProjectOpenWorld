#include "Pal/Widget/PalBox/PalInventorySlot.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/UniformGridSlot.h"
#include "Pal/Character/PalBaseCreature.h"
#include "Pal/Subsystem/PalCharacterDataSubsystem.h"
#include "Pal/Widget/PalBox/PalBoxWidget.h"
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Pal/Widget/PalBox/PalBoxDDO.h"
#include "GameBase/Subsystem/UIDataGameInstanceSubsystem.h"
#include "Pal/Component/PalStorageComponent.h"
#include "Pal/Component/PalSpawnerComponent.h"

void UPalInventorySlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UPalInventorySlot::NativeConstruct()
{
	UObject* OuterWidget = GetOuter();
	while (OuterWidget && !ParentWidget)
	{
		ParentWidget = Cast<UPalBoxWidget>(OuterWidget);
		OuterWidget = OuterWidget->GetOuter();
	}
	Super::NativeConstruct();
	if (!IsHovered() && SlotFrameImage)
	{
		SlotFrameImage->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UPalInventorySlot::SetPalCreature(APalBaseCreature* SelectedCreature)
{
	CurrentSelectedCreature = SelectedCreature;
	SlotImage->SetColorAndOpacity({1,1,1,0});
	if (CurrentSelectedCreature.IsValid())
	{
		if(SlotImage)
		{
			UTexture2D* IconDataRow = UPalCharacterDataSubsystem::GetPalCharacterIconByName(CurrentSelectedCreature->GetPalName());
			if (IconDataRow)
			{
				SlotImage->SetColorAndOpacity({1,1,1,1});
			}
			SlotImage->SetBrushFromTexture(IconDataRow);
		}
	}
}

void UPalInventorySlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	if (IsHovered() && SlotFrameImage)
	{
		UUIDataGameInstanceSubsystem::PlayUIHoverSound();
		SlotFrameImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UPalInventorySlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	if (!IsHovered() && SlotFrameImage)
	{
		SlotFrameImage->SetVisibility(ESlateVisibility::Hidden);
	}
}

FReply UPalInventorySlot::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (CurrentSelectedCreature.IsValid() && IsHovered())
	{
		if (ParentWidget)
		{
			UUIDataGameInstanceSubsystem::PlayButtonClickSound();
			ParentWidget->OnPalSelectedChanged(CurrentSelectedCreature.Get());
		}
	}
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

FReply UPalInventorySlot::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

FReply UPalInventorySlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) && IsHovered())
	{
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UPalInventorySlot::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
}

void UPalInventorySlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	if (!OutOperation && CurrentSelectedCreature.IsValid())
	{
		UPalBoxDDO* DDo = Cast<UPalBoxDDO>(UWidgetBlueprintLibrary::CreateDragDropOperation(UPalBoxDDO::StaticClass()));
		OutOperation = DDo;
		DDo->IsFromInventory = true;
		DDo->Index = SlotIndex;
		if (ParentWidget)
		{
			DDo->PalSpawnerComponent = ParentWidget->GetPalSpawnerComponent();
			DDo->PalStorageComponent = ParentWidget->GetPalStorageComponent();
		}
	}
	if (OutOperation)
	{
		OutOperation->Payload = this;
		UPalInventorySlot* DragVisual = CreateWidget<UPalInventorySlot>(this, GetClass());
		OutOperation->DefaultDragVisual = DragVisual;
		if (DragVisual)
		{
			DragVisual->SetPalCreature(CurrentSelectedCreature.Get());
		}
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
	UPalBoxDDO* PalDDO = Cast<UPalBoxDDO>(InOperation);
	if (PalDDO && this != InOperation->Payload)
	{
		if (ParentWidget)
		{
			if (PalDDO->IsFromInventory)
			{
				ParentWidget->GetPalStorageComponent()->SwapStoredPals(PalDDO->Index, this->SlotIndex);
				return true;
			}
			else
			{
				UPalStorageComponent* OwnerCom = ParentWidget->GetPalStorageComponent();
				if (PalDDO->PalSpawnerComponent && OwnerCom)
				{
					AActor* pTo = OwnerCom->GetStoredPal(SlotIndex);
					AActor* pFrom = PalDDO->PalSpawnerComponent->GetPal(PalDDO->Index);
					OwnerCom->RemovePal(SlotIndex);
					PalDDO->PalSpawnerComponent->RemovePal(PalDDO->Index);
					if (pFrom)
					{
						OwnerCom->StorePal({ pFrom,SlotIndex });
					}

					if (pTo)
					{
						PalDDO->PalSpawnerComponent->StorePal(pTo, PalDDO->Index);
					}
					return true;
				}
			}
		}

	}
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UPalInventorySlot::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
}
