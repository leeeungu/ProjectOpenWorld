#include "Item/Widget/PalItemRecipeMaterialWidget.h"
#include "Item/DataTable/PalItemRecipe.h"
#include "Item/DataTable/PalStaticItemDataStruct.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Player/Controller/BasePlayerController.h"
#include "Inventory/Component/InventoryComponent.h"
#include "GameBase/FunctionLib/StringTableFunctionLibrary.h"

void UPalItemRecipeMaterialWidget::SetMaterialData( FName InMaterialID, int InMaterialCount)
{
	Material_ID = InMaterialID;
	Material_Count = InMaterialCount;
	if (Material_ID == NAME_None)
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

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
			ItemName = UStringTableFunctionLibrary::GetItemNameFromStringTable(*ItemName.ToString());
			ItemNameText->SetText(ItemName);
		}
		
		UpdateMaterialWidget();

		if (ItemIconImage)
		{
			UTexture2D* IconTexture = UItemDataSubsystem::GetPalItemIconTextureByName(*Result->IconName);
			if (IconTexture)
			{
				ItemIconImage->SetBrushFromTexture(IconTexture);
			}
		}
	}
}

void UPalItemRecipeMaterialWidget::SetWidgetSize(FVector2D InSize)
{
	if (MaterialSizeBox)
	{
		MaterialSizeBox->SetWidthOverride(InSize.X);
		MaterialSizeBox->SetHeightOverride(InSize.Y);
	}
}

void UPalItemRecipeMaterialWidget::UpdateMaterialWidget()
{
	if (MaterialCountText)
	{
		int ItemCount{};
		ABasePlayerController* PC = Cast<ABasePlayerController>(GetOwningPlayer());
		if (PC)
		{
			if (UInventoryComponent* Inventory = PC->GetInventoryComponent())
			{
				ItemCount = Inventory->GetItemCount(Material_ID);
			}
		}
		FText Text = FText::FromString("/");
		MaterialCountText->SetText(FText::Format(FText::FromString("{0}{1}{2}"), FText::AsNumber(ItemCount), Text, FText::AsNumber(Material_Count)));
		if (ItemCount >= Material_Count)
		{
			MaterialCountText->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
		}
		else
		{
			MaterialCountText->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
		}
	}
}

bool UPalItemRecipeMaterialWidget::HasMaterial() const
{
	int ItemCount{};
	ABasePlayerController* PC = Cast<ABasePlayerController>(GetOwningPlayer());
	if (PC)
	{
		if (UInventoryComponent* Inventory = PC->GetInventoryComponent())
		{
			ItemCount = Inventory->GetItemCount(Material_ID);
		}
	}
	return ItemCount >= Material_Count;
}

int UPalItemRecipeMaterialWidget::GetMakeableCount() const
{
	int ItemCount{};
	ABasePlayerController* PC = Cast<ABasePlayerController>(GetOwningPlayer());
	//UE_LOG(LogTemp, Warning, TEXT("UPalItemRecipeMaterialWidget::GetMakeableCount - Material_ID: %s, Material_Count: %d"), *Material_ID.ToString(), Material_Count);
	if (PC && Material_Count > 0)
	{
		if (UInventoryComponent* Inventory = PC->GetInventoryComponent())
		{
			ItemCount = Inventory->GetItemCount(Material_ID);
			return ItemCount / Material_Count;
		}
	}
	return 0;
}
