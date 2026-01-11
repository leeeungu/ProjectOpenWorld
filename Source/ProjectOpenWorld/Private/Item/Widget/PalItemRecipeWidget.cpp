#include "Item/Widget/PalItemRecipeWidget.h"
#include "Item/DataTable/PalStaticItemDataStruct.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Item/DataTable/PalItemRecipe.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/Image.h"
#include "Item/Widget/PalItemRecipeMaterialWidget.h"

void UPalItemRecipeWidget::SetItemRecipe(const FName& InItemRecipe)
{
	ItemRecipe_ID = InItemRecipe;
	if (!UItemDataSubsystem::IsValidInstance())
		return;
	const FPalItemRecipe* Recipe{};
	if (!UItemDataSubsystem::GetPalItemRecipeDataPtr(ItemRecipe_ID, Recipe) || !Recipe)
		return;

	const FPalStaticItemDataStruct* Result{};
	if (UItemDataSubsystem::GetPalStaticItemDataPtr(ItemRecipe_ID, Result) && Result)
	{
		if (ItemNameText)
		{
			FText ItemName = FText::FromString(
				Result->OverrideName == "None" ? ItemRecipe_ID.ToString() :
				Result->OverrideName
			);
			ItemNameText->SetText(FText::Format(FText::FromString("{0} x {1}"), ItemName, FText::AsNumber(Recipe->Product_Count)));
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
	if (MaterialList)
	{
		int32 Count = MaterialList->GetChildrenCount();
		const TArray<FRecipeMaterialData>& RecipeMaterials = Recipe->Materials;
		for (int32 i = 0 ; i< Count; ++i)
		{
			UPalItemRecipeMaterialWidget* ChildWidget = Cast< UPalItemRecipeMaterialWidget>(MaterialList->GetChildAt(i));
			if (ChildWidget)
			{
				if (RecipeMaterials.IsValidIndex(i))
				{
					const FRecipeMaterialData& Mat = RecipeMaterials[i];
					ChildWidget->SetMaterialData(FName(*Mat.Material_Id), Mat.Material_Count);
				}
				else
				{
					ChildWidget->SetMaterialData(NAME_None,0);
				}
			}
		}
	}
}
