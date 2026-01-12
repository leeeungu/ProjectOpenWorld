#include "Item/Widget/PalItemRecipeWidget.h"
#include "Item/DataTable/PalStaticItemDataStruct.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Item/DataTable/PalItemRecipe.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "Components/EditableText.h"
#include "Item/Widget/PalItemRecipeMaterialWidget.h"
#include "Player/Controller/BasePlayerController.h"
#include "Inventory/Component/InventoryComponent.h"

void UPalItemRecipeWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (MakeButton)
	{
		MakeButton->OnClicked.AddDynamic(this, &UPalItemRecipeWidget::OnMakeButtonClicked);
	}
	if (MakeItemCountSlider)
	{
		MakeItemCountSlider->OnValueChanged.AddDynamic(this, &UPalItemRecipeWidget::OnSliderChanged);
		MakeItemCountSlider->SetStepSize(1.0f);
	}
	if (MakeItemCountText)
	{
		MakeItemCountText->OnTextChanged.AddDynamic(this, &UPalItemRecipeWidget::OnChangeMakeItemCountText);
	}
}

void UPalItemRecipeWidget::OnSliderChanged(float InValue)
{
	if (MakeItemCountText)
	{
		MakeItemCountText->SetText(FText::AsNumber(static_cast<int>(InValue)));
	}
	if (!FMath::IsNearlyZero(InValue - static_cast<int>(InValue)))
	{
		MakeItemCountSlider->SetValue(static_cast<int>(InValue));
	}
}

void UPalItemRecipeWidget::OnMakeButtonClicked()
{
	ABasePlayerController* PC = Cast<ABasePlayerController>(GetOwningPlayer());
	if (!PC || !MakeItemCountSlider)
	{
		return;
	}
	MakeItemCount = static_cast<int>(MakeItemCountSlider->GetValue());
	if (UInventoryComponent* Inventory = PC->GetInventoryComponent())
	{
		if (MaterialList)
		{
			bool bCanMake = true;
			int32 Count = MaterialList->GetChildrenCount();
			for (int32 i = 0; i < Count; ++i)
			{
				UPalItemRecipeMaterialWidget* ChildWidget = Cast< UPalItemRecipeMaterialWidget>(MaterialList->GetChildAt(i));
				if (ChildWidget)
				{
					if (!ChildWidget->HasMaterial())
						bCanMake = false;
				}
			}
			if (bCanMake)
			{
				int nMaxCount{INT32_MAX};
				for (int32 i = 0; i < Count; ++i)
				{
					UPalItemRecipeMaterialWidget* ChildWidget = Cast< UPalItemRecipeMaterialWidget>(MaterialList->GetChildAt(i));
					if (ChildWidget)
					{
						Inventory->RemoveItem(ChildWidget->GetMaterialID(), ChildWidget->GetMaterialCount() * MakeItemCount);
						ChildWidget->UpdateMaterialWidget();
						if (ChildWidget->GetMaterialID() != NAME_None)
							nMaxCount = FMath::Min(nMaxCount, ChildWidget->GetMakeableCount());
					}
				}
				Inventory->AddItem(ItemRecipe_ID, MakeItemCount);

				int nMinCount{};
				if (nMaxCount > 0)
					nMinCount = 1;
				if (MakeItemMaxCountText)
				{
					MakeItemMaxCountText->SetText(FText::AsNumber(nMaxCount));
				}
				if (MakeItemCountSlider)
				{
					MakeItemCountSlider->SetMinValue(nMinCount);
					MakeItemCountSlider->SetMaxValue(nMaxCount);
					MakeItemCountSlider->SetValue(nMinCount);
				}
				if (MakeItemMinCountText)
				{
					MakeItemMinCountText->SetText(FText::AsNumber(nMinCount));
				}
			}
			else
				UE_LOG(LogTemp, Warning, TEXT("UPalItemRecipeWidget::OnMakeButtonClicked - Not enough materials to make the item"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UPalItemRecipeWidget::OnMakeButtonClicked - Owning Player does not have an InventoryComponent"));
	}
}

void UPalItemRecipeWidget::OnChangeMakeItemCountText(const FText& InText)
{
	if (MakeItemCountSlider)
	{
		int32 NewCount = FCString::Atoi(*InText.ToString());
		if (NewCount < MakeItemCountSlider->GetMinValue())
		{
			NewCount = static_cast<int32>(MakeItemCountSlider->GetMinValue());
			MakeItemCountText->SetText(FText::AsNumber(NewCount));
		}
		else if (NewCount > MakeItemCountSlider->GetMaxValue())
		{
			NewCount = static_cast<int32>(MakeItemCountSlider->GetMaxValue());
			MakeItemCountText->SetText(FText::AsNumber(NewCount));
		}
		MakeItemCountSlider->SetValue(NewCount);
	}
}

void UPalItemRecipeWidget::SetItemRecipe( FName InItemRecipe)
{
	ItemRecipe_ID = InItemRecipe;
	if (ItemRecipe_ID == NAME_None)
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
			UTexture2D* IconTexture = UItemDataSubsystem::GetPalItemIconTextureByName(*Result->IconName);
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
		int nMaxCount{ INT32_MAX };
		for (int32 i = 0; i < Count; ++i)
		{
			UPalItemRecipeMaterialWidget* ChildWidget = Cast< UPalItemRecipeMaterialWidget>(MaterialList->GetChildAt(i));
			if (ChildWidget)
			{
				if (RecipeMaterials.IsValidIndex(i))
				{
					const FRecipeMaterialData& Mat = RecipeMaterials[i];
					ChildWidget->SetMaterialData(FName(*Mat.Material_Id), Mat.Material_Count);
					nMaxCount = FMath::Min(nMaxCount, ChildWidget->GetMakeableCount());
				}
				else
				{
					ChildWidget->SetMaterialData(NAME_None, 0);
				}
			}
		}
		int nMinCount{};
		if (nMaxCount > 0)
			nMinCount = 1;
		if (MakeItemMaxCountText)
		{
			MakeItemMaxCountText->SetText(FText::AsNumber(nMaxCount));
		}
		if (MakeItemCountSlider)
		{
			MakeItemCountSlider->SetMinValue(nMinCount);
			MakeItemCountSlider->SetMaxValue(nMaxCount);
			MakeItemCountSlider->SetValue(nMinCount);
		}
		if (MakeItemMinCountText)
		{
			MakeItemMinCountText->SetText(FText::AsNumber(nMinCount));
		}
		if (WorkingAmountText)
		{
			WorkingAmountText->SetText(FText::Format(FText::FromString("{0}"), FText::AsNumber(Recipe->WorkAmount / 100.0f))); // ÀÛ¾÷·®")));
		}
	}
}
