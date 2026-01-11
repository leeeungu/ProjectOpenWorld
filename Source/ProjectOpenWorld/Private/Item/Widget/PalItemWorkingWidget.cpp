#include "Item/Widget/PalItemWorkingWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Item/Widget/PalItemRecipeWidget.h"
#include "Item/Widget/PalItemRecipeSlot.h"
#include "Blueprint/WidgetTree.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UPalItemWorkingWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (RecipeUniformGrid && RecipeWidgetClass && WidgetTree)
	{
		RecipeUniformGrid->ClearChildren();
		int32 Count = 5 * 9;
		int i = 0;
		for (int Row = 0; Row < 9; Row++)
		{
			for (int Col = 0; Col < 5; Col++)
			{
				UPalItemRecipeSlot* NewRecipeSlot = CreateWidget<UPalItemRecipeSlot>(this, RecipeWidgetClass);
				if (NewRecipeSlot)
				{
					UUniformGridSlot* GridSlot = RecipeUniformGrid->AddChildToUniformGrid(NewRecipeSlot, Row, Col);
					GridSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
					GridSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
					if (RecipeIDList.IsValidIndex(i))
					{
						NewRecipeSlot->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
						NewRecipeSlot->SetRecipeID(RecipeIDList[i]);
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("UPalItemWorkingWidget::NativeOnInitialized Failed to Create Recipe Slot Widget"));
				}
				i++;
			}
		}
	}
}

void UPalItemWorkingWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (RecipeUniformGrid && RecipeWidgetClass && WidgetTree)
	{
		RecipeUniformGrid->ClearChildren();
		int32 Count = 5 * 9;
		int i = 0;
		for (int Row = 0; Row < 9; Row++)
		{
			for (int Col = 0; Col < 5; Col++)
			{
				UPalItemRecipeSlot* NewRecipeSlot = CreateWidget<UPalItemRecipeSlot>(this, RecipeWidgetClass);
				if (NewRecipeSlot)
				{
					UUniformGridSlot* GridSlot = RecipeUniformGrid->AddChildToUniformGrid(NewRecipeSlot, Row, Col);
					GridSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
					GridSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
					if (RecipeIDList.IsValidIndex(i))
					{
						NewRecipeSlot->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
						NewRecipeSlot->SetRecipeID(RecipeIDList[i]);
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("UPalItemWorkingWidget::NativeOnInitialized Failed to Create Recipe Slot Widget"));
				}
				i++;
			}
		}
	}
}

void UPalItemWorkingWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (RecipeUniformGrid)
	{
		int32 Count = RecipeUniformGrid->GetChildrenCount();
		for (int i = 0; i < Count; i++)
		{
			UPalItemRecipeSlot* RecipeSlot = Cast<UPalItemRecipeSlot>(RecipeUniformGrid->GetChildAt(i));
			if (RecipeSlot && RecipeIDList.IsValidIndex(i))
			{
				RecipeSlot->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				RecipeSlot->SetRecipeID(RecipeIDList[i]);
			}
			else
			{
				RecipeUniformGrid->GetChildAt(i)->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
	SetSelectedRecipeWidget(NAME_None);
}

void UPalItemWorkingWidget::SetSelectedRecipeWidget(FName SelectRecipeID)
{
	if (SelectRecipeWidget)
	{
		SelectRecipeWidget->SetItemRecipe(SelectRecipeID);
	}
}
