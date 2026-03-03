#include "Building/Widget/BuildingDescWidget.h"
#include "Building/Subsystem/BuildingDataSubsystem.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Item/Widget/PalItemRecipeMaterialWidget.h"
#include "GameBase/FunctionLib/StringTableFunctionLibrary.h"

void UBuildingDescWidget::UpdateNameText()
{
	if (!BuildNameText)
		return;
	FText BuildName = FText::FromName(BuildObjectId);
	BuildName = UStringTableFunctionLibrary::GetBuildingNameFromStringTable(*BuildName.ToString());
	BuildNameText->SetText(BuildName);
}

void UBuildingDescWidget::UpdateDescText()
{
	if (!BuildDescText)
		return;
	FText BuildDesc = FText::FromName(BuildObjectId);
	BuildDesc = UStringTableFunctionLibrary::GetBuildingDescFromStringTable(*BuildDesc.ToString());
	BuildDescText->SetText(BuildDesc);
}

void UBuildingDescWidget::UpdateRecipeVericalBox()
{
	if (!RecipeMaterialVerticalBox)
		return;
	TArray<FBuildMaterialData> Materials =	UBuildingDataSubsystem::GetPalBuildObjectMaterialsByName(BuildObjectId);
	int VerChildSize =  RecipeMaterialVerticalBox->GetChildrenCount();
	for (int i = 0; i < VerChildSize; i++)
	{
		UPalItemRecipeMaterialWidget* MaterialWidget = Cast< UPalItemRecipeMaterialWidget>(RecipeMaterialVerticalBox->GetChildAt(i));
		//RecipeMaterialVerticalBox->GetChildAt(i)->RemoveFromParent();
		if (MaterialWidget)
		{
			if (Materials.IsValidIndex(i))
			{
				MaterialWidget->SetMaterialData(Materials[i].Material_Id, Materials[i].Material_Count);
				MaterialWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
			else
			{
				MaterialWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}

void UBuildingDescWidget::SetBuildingDescData(FName InBuildObjectId)
{
	BuildObjectId = InBuildObjectId;
	if(BuildObjectId == NAME_None)
	{
		SetVisibility(ESlateVisibility::Hidden);
		return;
	}
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	UpdateNameText();
	UpdateDescText();
	UpdateRecipeVericalBox();
}
