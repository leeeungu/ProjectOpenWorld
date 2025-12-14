#include "Building/Widget/BuildingModeImage.h"
#include "Components/Button.h"

#include "Blueprint/WidgetBlueprintLibrary.h"

UBuildingModeImage::UBuildingModeImage() : UButton{}
{
	OnClicked.AddDynamic(this, &UBuildingModeImage::OnStartBuilding);
	OnHovered.AddDynamic(this, &UBuildingModeImage::OnSelectBuilding);
}

void UBuildingModeImage::OnStartBuilding()
{
	UE_LOG(LogTemp, Warning, TEXT("UBuildingModeImage::OnSelectBuilding SlotIndex:%d"), SlotIndex);
}

void UBuildingModeImage::OnSelectBuilding()
{
	UE_LOG(LogTemp, Warning, TEXT("UBuildingModeImage::OnSelectBuilding SlotIndex:%d"), SlotIndex);
	if (!ParentWidget)
		return;
	return;
}
