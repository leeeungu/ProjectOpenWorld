#include "Inventory/Widget/InventorySlotToolTip.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Item/DataTable/PalStaticItemDataStruct.h"
#include "GameBase/FunctionLib/StringTableFunctionLibrary.h"


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
		if (ItemNameText)
		{
			FText ItemName = FText::FromString(
				Result->OverrideName == "None" ? RecipeID.ToString() :
				Result->OverrideName
			);
			ItemName = UStringTableFunctionLibrary::GetItemNameFromStringTable(*ItemName.ToString());
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
			FText Desc = FText::FromString(
				Result->OverrideName == "None" ? RecipeID.ToString() :
				Result->OverrideName
			);
			Desc = UStringTableFunctionLibrary::GetItemDescFromStringTable(*Desc.ToString());
			ItemDescText->SetText(Desc);
		}
	}
}
