#include "Item/Widget/PalItemRecipeToolTip.h"
#include "Item/Widget/PalItemRecipeMaterialWidget.h"
#include "Item/DataTable/PalStaticItemDataStruct.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/VerticalBoxSlot.h"
#include "Blueprint/WidgetTree.h"
#include "GameBase/FunctionLib/StringTableFunctionLibrary.h"

void UPalItemRecipeToolTip::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPalItemRecipeToolTip::SetRecipeID(FName InRecipeID)
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
		if (RecipeNameText)
		{
			FText ItemName = FText::FromString(
				Result->OverrideName == "None" ? RecipeID.ToString() :
				Result->OverrideName
			);
			ItemName = UStringTableFunctionLibrary::GetItemNameFromStringTable(*ItemName.ToString());
			RecipeNameText->SetText(ItemName);
		}
		if (RecipeIconImage)
		{
			UTexture2D* IconTexture = UItemDataSubsystem::GetPalItemIconTextureByName(*Result->IconName);
			if (IconTexture)
			{
				RecipeIconImage->SetBrushFromTexture(IconTexture);
			}
		}
		if (RecipeDescText)
		{
			FText Desc = FText::FromString(
				Result->OverrideName == "None" ? RecipeID.ToString() :
				Result->OverrideName
			);
			Desc = UStringTableFunctionLibrary::GetItemDescFromStringTable(*Desc.ToString());
			RecipeDescText->SetText(Desc);
		}
	}

	if (MaterialList)
	{
		int32 Count = MaterialList->GetChildrenCount();
		const FPalItemRecipe* Recipe{};
		UItemDataSubsystem::GetPalItemRecipeDataPtr(RecipeID, Recipe);
		for (int32 i = 0; i < Count; i++)
		{
			UPalItemRecipeMaterialWidget* MaterialWidget = Cast<UPalItemRecipeMaterialWidget>(MaterialList->GetChildAt(i));
			if (MaterialWidget )
			{
				MaterialWidget->SetWidgetSize(FVector2D(300, 48));
				FRecipeMaterialData MaterialData{};
				if(Recipe && Recipe->Materials.IsValidIndex(i))
					MaterialData = Recipe->Materials[i];
				MaterialWidget->SetMaterialData(*MaterialData.Material_Id, MaterialData.Material_Count);
			}
		}
	}
}


void UPalItemRecipeToolTip::UpdateToolTipWidget()
{
	if (MaterialList)
	{
		int32 Count = MaterialList->GetChildrenCount();
		for (int32 i = 0; i < Count; i++)
		{
			UPalItemRecipeMaterialWidget* MaterialWidget = Cast<UPalItemRecipeMaterialWidget>(MaterialList->GetChildAt(i));
			if (MaterialWidget)
			{
				MaterialWidget->UpdateMaterialWidget();
			}
		}
	}
}
