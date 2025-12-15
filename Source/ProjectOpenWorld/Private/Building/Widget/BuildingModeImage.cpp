#include "Building/Widget/BuildingModeImage.h"
#include "Building/Widget/BuildingModeWidget.h"
#include "Components/Button.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

UBuildingModeImage::UBuildingModeImage() : UButton{}
{
}

void UBuildingModeImage::OnStartBuilding()
{
	if (!ParentWidget || !BuildingMesh)
		return;
	ParentWidget->StartBuildingMode(BuildingMesh);
}

void UBuildingModeImage::OnHoverBuilding()
{
	if (!ParentWidget)
		return;
	if (IsHovered())
	{
		ParentWidget->HoverBuildingMode(SlotIndex);
	}
	else
	{
		ParentWidget->HoverBuildingMode(-1);
	}
	//ParentWidget->
}

void UBuildingModeImage::ChangeButtonImage()
{
	FButtonStyle style = GetStyle();
	FSlateBrush Image{};
	Image.SetResourceObject(BuildingTexture);
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
	OnUnhovered.AddUniqueDynamic(this, &UBuildingModeImage::OnHoverBuilding);
	ChangeButtonImage();
}

void UBuildingModeImage::SetSlotIndex(uint8 Index)
{
	SlotIndex = Index;
}

void UBuildingModeImage::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UButton::PostEditChangeProperty(PropertyChangedEvent);
	ChangeButtonImage();
}
