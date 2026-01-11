#include "Item/Widget/PalItemRecipeWidget.h"
#include "Item/DataTable/PalStaticItemDataStruct.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Item/DataTable/PalItemRecipe.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Item/Widget/PalItemRecipeMaterialWidget.h"

void UPalItemRecipeWidget::SetItemRecipe(const FName& InItemRecipe)
{
	ItemRecipe_ID = InItemRecipe;
	if (!UItemDataSubsystem::IsValidInstance())
		return;
	const FPalStaticItemDataStruct* Result{};
	if (UItemDataSubsystem::GetPalStaticItemDataPtr(ItemRecipe_ID, Result))
	{
		if (ItemNameText)
		{
			FText ItemName = FText::FromString(
					Result->OverrideName == "None" ? ItemRecipe_ID.ToString() :
					Result->OverrideName
				);
				ItemNameText->SetText(ItemName);
		}
		if (ItemIconImage)
		{
			UTexture2D* IconTexture = UItemDataSubsystem::GetPalItemIconTextureByName(ItemRecipe_ID);
			if (IconTexture)
			{
				ItemIconImage->SetBrushFromTexture(IconTexture);
			}
		}
	}
	TArray<FRecipeMaterialData> RecipeMaterials = UItemDataSubsystem::GetPalItemRecipeMaterialsByName(ItemRecipe_ID);
	for (size_t i = 0; i < length; i++)
	{
	}
}
