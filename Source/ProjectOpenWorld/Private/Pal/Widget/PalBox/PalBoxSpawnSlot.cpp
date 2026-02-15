#include "Pal/Widget/PalBox/PalBoxSpawnSlot.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/CanvasPanel.h"
#include "Components/VerticalBoxSlot.h"
#include "Creature/Character/BaseCreature.h"
#include "Pal/Subsystem/PalCharacterDataSubsystem.h"
#include "Pal/Widget/PalBox/PalBoxDDO.h"
#include "Pal/Widget/PalBox/PalBoxWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GameBase/Widget/UserWidgetTemplate.h"
#include "GameBase/Subsystem/UIDataGameInstanceSubsystem.h"


void UPalBoxSpawnSlot::NativeConstruct()
{
	Super::NativeConstruct();
	SetPalCreature(nullptr);	
	if(BackgroundFrameImage)
		BackgroundFrameImage->SetColorAndOpacity(FLinearColor::Gray);
}

void UPalBoxSpawnSlot::NativeDestruct()
{
	Super::NativeDestruct();
	//UE_LOG(LogTemp, Log, TEXT("PalBoxSpawnSlot: Destructed"));
}

void UPalBoxSpawnSlot::SetPalCreature(ABaseCreature* InCreature)
{
	CurrentSelectedCreature = InCreature;
	PalInfoCanvasPanel->SetVisibility(ESlateVisibility::Hidden);
	if (CurrentSelectedCreature.IsValid())
	{
		PalInfoCanvasPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		FText CharacterID = FText::FromName(CurrentSelectedCreature->GetPalID());
		//Script/Engine.StringTable'/Game/Pal/StringTable/ST_PalName.ST_PalName'

		CharacterID = FText::FromStringTable("/Game/Pal/StringTable/ST_PalName.ST_PalName", *CharacterID.ToString());
		NameText->SetText(CharacterID);
		LevelText->SetText(FText::AsNumber(1)); 
		UTexture2D* IconDataRow = UPalCharacterDataSubsystem::GetPalCharacterIconByName(CurrentSelectedCreature->GetPalID());
		IconImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		if (IconDataRow)
		{
			IconImage->SetBrushFromTexture(IconDataRow);
		}
		else
		{
			IconImage->SetBrushFromTexture(nullptr); 
		}
		HpProgressBar->SetPercent(0.5f); 
	}
}

void UPalBoxSpawnSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	//(R=0.000000,G=0.004000,B=0.015000,A=0.800000)
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent); 
	UUIDataGameInstanceSubsystem::PlayUIHoverSound();
	if (BackgroundFrameImage)
		BackgroundFrameImage->SetColorAndOpacity(WidgetTemplate::UIBackGround);
}

void UPalBoxSpawnSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	//(R = 0.5, G = 0.5, B = 0.5, A = 0.100000)
	Super::NativeOnMouseLeave(InMouseEvent); 
	if (BackgroundFrameImage)
		BackgroundFrameImage->SetColorAndOpacity(FLinearColor::Gray);
}

FReply UPalBoxSpawnSlot::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (CurrentSelectedCreature.IsValid() && IsHovered())
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
			UUIDataGameInstanceSubsystem::PlayButtonClickSound();
			ParentWidget->OnPalSelectedChanged(CurrentSelectedCreature.Get());
		}
	}
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

FReply UPalBoxSpawnSlot::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseMove(InGeometry, InMouseEvent);
	return FReply::Handled();
}

FReply UPalBoxSpawnSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) && IsHovered())
	{
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	return FReply::Unhandled();
}

FReply UPalBoxSpawnSlot::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
	return FReply::Handled();
}

void UPalBoxSpawnSlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	if (!OutOperation)
	{
		UPalBoxDDO* DDo = Cast< UPalBoxDDO>(UWidgetBlueprintLibrary::CreateDragDropOperation(UPalBoxDDO::StaticClass()));
		OutOperation = DDo;
		DDo->IsFromInventory = false;
		DDo->Index = SlotIndex;
	}
	if (OutOperation)
	{
		OutOperation->Payload = this;
		UPalBoxSpawnSlot* DragVisual = CreateWidget<UPalBoxSpawnSlot>(this, GetClass());
		OutOperation->DefaultDragVisual = DragVisual;
		if (DragVisual)
		{
			DragVisual->SetPalCreature(CurrentSelectedCreature.Get());
		}
	}
}

bool UPalBoxSpawnSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UPalBoxDDO* DDo = Cast< UPalBoxDDO>(InOperation);
	if (DDo)
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
			//UE_LOG(LogTemp, Log, TEXT("PalBoxSpawnSlot: On Drop Detected, FromInventory=%d, Index=%d, ToSlotIndex=%d"), DDo->IsFromInventory, DDo->Index, this->SlotIndex);
			if (DDo->IsFromInventory)
			{
				ParentWidget->SpawnSlotFromInventory(DDo->Index, this->SlotIndex);
				return true;
			}
			else
			{
				ParentWidget->SwapSpawnInventory(DDo->Index, this->SlotIndex);
				return true;
			}
		}
	}
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}