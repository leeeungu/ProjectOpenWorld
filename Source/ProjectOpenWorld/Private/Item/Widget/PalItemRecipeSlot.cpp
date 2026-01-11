#include "Item/Widget/PalItemRecipeSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Item/DataTable/PalItemRecipe.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Item/Widget/PalItemWorkingWidget.h"
#include "Blueprint/WidgetTree.h"

void UPalItemRecipeSlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ItemButton->OnClicked.AddDynamic(this, &UPalItemRecipeSlot::OnItemButtonClicked);
}

void UPalItemRecipeSlot::NativeConstruct()
{
	Super::NativeConstruct();
	if (GetOuter())
	{
		OwningWorkingWidget = Cast<UPalItemWorkingWidget>(GetOuter());
		if (!OwningWorkingWidget)
		{
			OwningWorkingWidget = Cast<UPalItemWorkingWidget>(GetOuter()->GetOuter());
		}
	}
}


void UPalItemRecipeSlot::OnItemButtonClicked()
{
	if (OwningWorkingWidget && OwningWorkingWidget.Get())
	{
		OwningWorkingWidget->SetSelectedRecipeWidget(RecipeID);
	}
}

void UPalItemRecipeSlot::SetRecipeID(const FName& InRecipeID)
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
	const FPalItemRecipe* ItemRecipe{};
	const FPalStaticItemDataStruct* Result{};
	if (UItemDataSubsystem::GetPalStaticItemDataPtr(RecipeID, Result) && Result)
	{
		if (UItemDataSubsystem::GetPalItemRecipeDataPtr(RecipeID, ItemRecipe) && ItemRecipe)
		{
			if (ItemIcon)
			{
				ItemIcon->SetBrushFromTexture(UItemDataSubsystem::GetPalItemIconTextureByName(*Result->IconName));
			}
			if (ItemCountText)
			{
				if (ItemRecipe->Product_Count > 1)
				{
					ItemCountText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
					ItemCountText->SetText(FText::Format(FText::FromString("x {0}"), FText::AsNumber(ItemRecipe->Product_Count)));
				}
				else
				{
					ItemCountText->SetVisibility(ESlateVisibility::Hidden);
				}
			}
		}
	}
}
