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
#include "Building/Widget/BuildingDescWidget.h"
#include "GameBase/Subsystem/UIDataGameInstanceSubsystem.h"

void UBuildingModeWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
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
	Super::NativePreConstruct();
	if (ButtonPanel)
		ButtonPanel->PreConstruct();
}

void UBuildingModeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ABasePlayer* Player = GetOwningPlayer()->GetPawn<ABasePlayer>();
	if (!Player)
		return;
	SelectBuilding(NAME_None);
	BuildingAssistComp = Player->GetBuildingAssist();
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
	HoverBuildingMode(-1);
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

void UBuildingModeWidget::StartViewWidget()
{
	APlayerController* pc = GetOwningPlayer();
	if (!pc || IsInViewport())
		return;
	AddToViewport();
	GetOwningPlayer()->SetShowMouseCursor(true);
	UUIDataGameInstanceSubsystem::PlayUIOpenSound();
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(GetOwningPlayer(), this, EMouseLockMode::DoNotLock, true, true);
	UGameplayStatics::SetViewportMouseCaptureMode(GetOwningPlayer()->GetWorld(), EMouseCaptureMode::NoCapture);
}

void UBuildingModeWidget::EndViewWidget()
{
	APlayerController* pc = GetOwningPlayer();
	if (!pc || !IsInViewport())
		return;
	RemoveFromParent();
	GetOwningPlayer()->SetShowMouseCursor(false);
	if(!StartedBuilding)
		UUIDataGameInstanceSubsystem::PlayUICloseSound();
	UGameplayStatics::SetViewportMouseCaptureMode(GetOwningPlayer()->GetWorld(), EMouseCaptureMode::CaptureDuringMouseDown);
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(GetOwningPlayer(), true);
	UWidgetBlueprintLibrary::CancelDragDrop();
	StartedBuilding = false;
}

void UBuildingModeWidget::SetBuildingUI(TArray<FName> BuildingIDs)
{
	if (ButtonPanel)
	{
		int32 Size = ButtonPanel->GetChildrenCount();
		for (int32 i = 1; i < Size; i++)
		{
			if (UBuildingModeImage* btn = Cast<UBuildingModeImage>(ButtonPanel->GetChildAt(i)))
			{
				btn->SetBuildingID(BuildingIDs.IsValidIndex(i - 1) ? BuildingIDs[i - 1] : NAME_None);
			}
		}
	}
}

void UBuildingModeWidget::SelectBuilding(FName BuildingID)
{
	if (BuildingDescWidget)
	{
		BuildingDescWidget->SetBuildingDescData(BuildingID);
	}
}

void UBuildingModeWidget::StartBuildingMode(FName BuildingID, UStaticMesh* Mesh)
{
	if (!BuildingAssistComp || !Mesh)
		return;
	StartedBuilding = true;
	BuildingAssistComp->SetBuildingStaticMesh(BuildingID, Mesh);
	BuildingAssistComp->StartBuilding();
}

void UBuildingModeWidget::HoverBuildingMode(int SlotIndex)
{
	if (!DynamicMaterial)
	{
		return;
	}
	DynamicMaterial->SetScalarParameterValue(TEXT("Selected"), SlotIndex);
}

