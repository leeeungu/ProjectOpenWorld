#include "Inventory/Widget/InventorySlotToolTip.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Item/DataTable/PalStaticItemDataStruct.h"


void UInventorySlotToolTip::NativeConstruct()
{
	Super::NativeConstruct();
}

void UInventorySlotToolTip::SetRecipeID(FName InRecipeID)
{
	RecipeID = InRecipeID;
	if (RecipeID == NAME_None)
	{
		SetVisibility(ESlateVisibility::Hidden);
		return;
	}
	else
	{
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	if (!UItemDataSubsystem::IsValidInstance())
		return;
	const FPalStaticItemDataStruct* Result{};
	if (UItemDataSubsystem::GetPalStaticItemDataPtr(RecipeID, Result) && Result)
	{
		// Further implementation can be added here to update the tooltip UI based on the RecipeID
		if (ItemNameText)
		{
			FText ItemName = FText::FromString(
				Result->OverrideName == "None" ? RecipeID.ToString() :
				Result->OverrideName
			);
			ItemNameText->SetText(ItemName);
		}
		if (ItemIconImage)
		{
			UTexture2D* IconTexture = UItemDataSubsystem::GetPalItemIconTextureByName(*Result->IconName);
			if (IconTexture)
			{
				ItemIconImage->SetBrushFromTexture(IconTexture);
			}
		}
		if (ItemDescText)
		{
			ItemDescText->SetText(FText::FromString(
				Result->OverrideDescription == "None" ? FString("No Description Available") :
				Result->OverrideDescription
						));
		}
	}
}
