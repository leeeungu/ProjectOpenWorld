#include "Item/Widget/PalItemRecipeMaterialWidget.h"
#include "Item/DataTable/PalItemRecipe.h"
#include "Item/DataTable/PalStaticItemDataStruct.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UPalItemRecipeMaterialWidget::SetMaterialData(const FName& InMaterialID, int InMaterialCount)
{
	Material_ID = InMaterialID;
	Material_Count = InMaterialCount;
	if (!UItemDataSubsystem::IsValidInstance())
		return;
	const FPalStaticItemDataStruct* Result{};
	if (UItemDataSubsystem::GetPalStaticItemDataPtr(Material_ID, Result))
	{
		if (ItemNameText)
		{
			FText ItemName = FText::FromString(
					Result->OverrideName == "None" ? Material_ID.ToString() :
					Result->OverrideName
				);
				ItemNameText->SetText(ItemName);
		}

		if (MaterialCountText)
		{
			FText Text = FText::FromString("/");
			MaterialCountText->SetText(FText::Format(FText::FromString("{0}{1}{2}"),FText::AsNumber(10), Text, FText::AsNumber(Material_Count)));
		}

		if (ItemIconImage)
		{
			UTexture2D* IconTexture = UItemDataSubsystem::GetPalItemIconTextureByName(Material_ID);
			if (IconTexture)
			{
				ItemIconImage->SetBrushFromTexture(IconTexture);
			}
		}
	}
}
