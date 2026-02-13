#include "Building/Widget/BuildingModeImage.h"
#include "Building/Widget/BuildingModeWidget.h"
#include "Components/Button.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Building/Subsystem/BuildingDataSubsystem.h"
#include "GameBase/Subsystem/UIDataGameInstanceSubsystem.h"

UBuildingModeImage::UBuildingModeImage() : UButton{}
{
}

void UBuildingModeImage::OnStartBuilding()
{
	if (!ParentWidget || !BuildingMesh)
		return;
	UUIDataGameInstanceSubsystem::PlayUIBuildPressSound();
	ParentWidget->StartBuildingMode(BuildObjectId, BuildingMesh);
}

void UBuildingModeImage::OnHoverBuilding()
{
	if (!ParentWidget && !IsHovered())
		return;
	UUIDataGameInstanceSubsystem::PlayUIBuildHoverSound();
	ParentWidget->SelectBuilding(BuildObjectId);
	ParentWidget->HoverBuildingMode(SlotIndex);
}

void UBuildingModeImage::OnUnHoverBuilding()
{
	if (!ParentWidget)
		return;
	ParentWidget->SelectBuilding(NAME_None);
	ParentWidget->HoverBuildingMode(-1);
}

void UBuildingModeImage::ChangeButtonImage()
{
	FButtonStyle style = GetStyle();
	FSlateBrush Image{};
	{
		UTexture2D* IconTexture = UBuildingDataSubsystem::GetPalBuildObjectIconTextureByName(BuildObjectId);
		Image.SetResourceObject(IconTexture);
	}
	Image.DrawAs = ESlateBrushDrawType::Type::Image;
	style.SetNormal(Image);
	style.SetHovered(Image);
	style.SetPressed(Image);
	SetStyle(style);
}

void UBuildingModeImage::SetParent(UBuildingModeWidget* Parent)
{
	ParentWidget = Parent;
	OnClicked.AddUniqueDynamic(this, &UBuildingModeImage::OnStartBuilding);
	OnHovered.AddUniqueDynamic(this, &UBuildingModeImage::OnHoverBuilding);
	OnUnhovered.AddUniqueDynamic(this, &UBuildingModeImage::OnUnHoverBuilding);
	ChangeButtonImage();
}

void UBuildingModeImage::SetSlotIndex(uint8 Index)
{
	SlotIndex = Index;
}

void UBuildingModeImage::SetBuildingID(FName ID)
{
	BuildObjectId = ID;
	BuildingTexture = UBuildingDataSubsystem::GetPalBuildObjectIconTextureByName(BuildObjectId);
	//BuildingMesh = UBuildingDataSubsystem::GetPalBuildObjectBuildingMeshByName(BuildObjectId);
	if (BuildObjectId == NAME_None || !BuildingTexture || !BuildingMesh)
	{
		SetVisibility(ESlateVisibility::Hidden);
		return;
	}
	SetVisibility(ESlateVisibility::Visible);
	ChangeButtonImage();
}

void UBuildingModeImage::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UButton::PostEditChangeProperty(PropertyChangedEvent);
	BuildingTexture = UBuildingDataSubsystem::GetPalBuildObjectIconTextureByName(BuildObjectId);
	ChangeButtonImage();
}
