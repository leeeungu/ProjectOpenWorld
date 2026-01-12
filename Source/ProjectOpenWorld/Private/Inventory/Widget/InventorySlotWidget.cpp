#include "Inventory/Widget/InventorySlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Item/DataTable/PalStaticItemDataStruct.h"

void UInventorySlotWidget::SetSlotData_Implementation(const FInventorySlot& Data)
{
	if (Data.isEmpthySlot || Data.ItemID == NAME_None)
	{
		if (ItemImage)
		{
			ItemImage->SetBrushFromTexture(nullptr);
			ItemImage->SetVisibility(ESlateVisibility::Hidden);
		}
		if (ItemWeight)
			ItemWeight->SetText(FText::FromString(""));
		if (ItemCount)
			ItemCount->SetText(FText::FromString(""));
		return;
	}
	if (!UItemDataSubsystem::IsValidInstance())
		return;
	const FPalStaticItemDataStruct* ItemData{};
	if (UItemDataSubsystem::GetPalStaticItemDataPtr(Data.ItemID, ItemData) && ItemData)
	{
		if (ItemImage)
		{
			ItemImage->SetBrushFromTexture(UItemDataSubsystem::GetPalItemIconTextureByName(*ItemData->IconName));
			ItemImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		float TotalWeight = ItemData->Weight * Data.ItemCount;
		if (ItemWeight)
			ItemWeight->SetText(FText::AsNumber(TotalWeight));
		if (ItemCount)
			ItemCount->SetText(FText::AsNumber(Data.ItemCount));
	}

}
