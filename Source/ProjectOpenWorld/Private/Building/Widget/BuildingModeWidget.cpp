#include "Building/Widget/BuildingModeWidget.h"
#include "Building/BuildingAssistComponent.h"
#include "Player/Character/BasePlayer.h" 
#include "Materials/MaterialInstance.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Building/Widget/BuildingModeImage.h"
#include "Building/Widget/BuildingModeCanvasPanel.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"


void UBuildingModeWidget::NativeOnInitialized()
{
	UUserWidget::NativeOnInitialized();
	ABasePlayer* Player = GetOwningPlayer()->GetPawn<ABasePlayer>();
	if (!Player)
		return;
	BuildingAssistComp = Player->GetBuildingAssist();
	if (SelectImage && !DynamicMaterial)
	{
		UMaterialInstance* BuildSelectMat = Cast< UMaterialInstance>(SelectImage->GetBrush().GetResourceObject());
		if (BuildSelectMat)
		{
			DynamicMaterial = UMaterialInstanceDynamic::Create(BuildSelectMat, this);
			SelectImage->SetBrushFromSoftMaterial(DynamicMaterial);
		}
	}
	if (UPanelSlot* RootPanel = Cast<UPanelSlot>(ButtonPanel->Slot))
	{
		for (int32 i = 0; i < ButtonPanel->GetChildrenCount(); i++)
		{
			if (UBuildingModeImage* btn = Cast<UBuildingModeImage>(ButtonPanel->GetChildAt(i)))
			{
				btn->SetParent(this);
			}
		}
	}
}

void UBuildingModeWidget::NativePreConstruct()
{
	UUserWidget::NativePreConstruct();
	if (ButtonPanel)
		ButtonPanel->PreConstruct();
}

void UBuildingModeWidget::NativeConstruct()
{
	UUserWidget::NativeConstruct();

	ABasePlayer* Player = GetOwningPlayer()->GetPawn<ABasePlayer>();
	if (!Player)
		return;
	BuildingAssistComp = Player->GetBuildingAssist();
	HoverBuildingMode(-1);
	if (ButtonPanel)
		ButtonPanel->PreConstruct();
	if (ButtonPanel)
	{
		if (UPanelSlot* RootPanel = Cast<UPanelSlot>(ButtonPanel->Slot))
		{
			for (int32 i = 0; i < ButtonPanel->GetChildrenCount(); i++)
			{
				if (UBuildingModeImage* btn = Cast<UBuildingModeImage>(ButtonPanel->GetChildAt(i)))
				{
					btn->SetParent(this);
				}
			}
		}
	}
	if (SelectImage && !DynamicMaterial)
	{
		UMaterialInstance* BuildSelectMat = Cast< UMaterialInstance>(SelectImage->GetBrush().GetResourceObject());
		if (BuildSelectMat)
		{
			DynamicMaterial = UMaterialInstanceDynamic::Create(BuildSelectMat, this);
			SelectImage->SetBrushFromSoftMaterial(DynamicMaterial);
		}
	}
}

void UBuildingModeWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UBuildingModeWidget::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (ButtonPanel)
		ButtonPanel->PreConstruct();
}

void UBuildingModeWidget::ToggleWidget()
{
	if (!GetIsVisible())
	{
		StartViewWidget();
	}
	else
	{
		EndViewWidget();
	}
}

void UBuildingModeWidget::StartViewWidget()
{
	AddToViewport();
	GetOwningPlayer()->SetShowMouseCursor(true);
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(GetOwningPlayer(), this, EMouseLockMode::DoNotLock, true, true);
	UGameplayStatics::SetViewportMouseCaptureMode(GetOwningPlayer()->GetWorld(), EMouseCaptureMode::NoCapture);
}

void UBuildingModeWidget::EndViewWidget()
{
	RemoveFromParent();
	GetOwningPlayer()->SetShowMouseCursor(false);
	UGameplayStatics::SetViewportMouseCaptureMode(GetOwningPlayer()->GetWorld(), EMouseCaptureMode::CaptureDuringMouseDown);
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(GetOwningPlayer(), true);
	UWidgetBlueprintLibrary::CancelDragDrop();
}

void UBuildingModeWidget::StartBuildingMode(FName BuildingID, UStaticMesh* Mesh)
{
	if (!BuildingAssistComp || !Mesh)
		return;
	BuildingAssistComp->SetBuildingStaticMesh(BuildingID, Mesh);
	BuildingAssistComp->StartBuilding();
	EndViewWidget();
}

void UBuildingModeWidget::HoverBuildingMode(int SlotIndex)
{
	if (!DynamicMaterial)
	{
		return;
	}
	DynamicMaterial->SetScalarParameterValue(TEXT("Selected"), SlotIndex);
}

