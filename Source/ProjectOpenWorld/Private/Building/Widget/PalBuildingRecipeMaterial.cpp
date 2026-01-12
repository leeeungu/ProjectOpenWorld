#include "Building/Widget/PalBuildingRecipeMaterial.h"
#include "Item/DataTable/PalStaticItemDataStruct.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Player/Controller/BasePlayerController.h"
#include "Inventory/Component/InventoryComponent.h"

void UPalBuildingRecipeMaterial::SetMaterialData(FName InMaterialId, int32 InMaterialCount)
{
	MaterialId = InMaterialId;
	MaterialCount = InMaterialCount;

	if (!MaterialCountText || !MaterialIconImage)
		return;
	if (InMaterialCount == 0)
	{
		MaterialCountText->SetVisibility(ESlateVisibility::Hidden);
		MaterialIconImage->SetVisibility(ESlateVisibility::Hidden);
		return;
	}
	else
	{
		MaterialCountText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		MaterialIconImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	if (MaterialIconImage)
	{
		if (UItemDataSubsystem::IsValidInstance())
		{
			const FPalStaticItemDataStruct* ItemData{};
			if (UItemDataSubsystem::GetPalStaticItemDataPtr(MaterialId, ItemData) && ItemData)
			{
				MaterialIconImage->SetBrushFromTexture(UItemDataSubsystem::GetPalItemIconTextureByName(*ItemData->IconName));
			}
		}
	}
	UpdateMaterialData();
}

void UPalBuildingRecipeMaterial::UpdateMaterialData()
{
	ABasePlayerController* PlayerController = Cast<ABasePlayerController>(GetOwningPlayer());
	if (PlayerController && MaterialCountText)
	{
		UInventoryComponent* InventoryComp = PlayerController->GetInventoryComponent();
		if (InventoryComp)
		{
			int HasCount = InventoryComp->GetItemCount(MaterialId);
			if (HasCount >= MaterialCount)
			{
				MaterialCountText->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
			}
			else
			{
				MaterialCountText->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
			}
			MaterialCountText->SetText(FText::Format(FText::FromString("{0} / {1}"), FText::AsNumber(HasCount), FText::AsNumber(MaterialCount)));
		}
	}
}
