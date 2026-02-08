#include "Building/Widget/BuildingDescWidget.h"
#include "Building/Subsystem/BuildingDataSubsystem.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Item/Widget/PalItemRecipeMaterialWidget.h"

void UBuildingDescWidget::UpdateNameText()
{
	if (!BuildNameText)
		return;
	FText BuildName = FText::FromName(BuildObjectId);
	//Script/Engine.StringTable'/Game/Global/StringTable/ST_BuildingName.ST_BuildingName'
	BuildName = FText::FromStringTable("/Game/Global/StringTable/ST_BuildingName", *BuildName.ToString());
	BuildNameText->SetText(BuildName);
}

void UBuildingDescWidget::UpdateDescText()
{
	if (!BuildDescText)
		return;
	FText BuildDesc = FText::FromName(BuildObjectId);
	//Script/Engine.StringTable'/Game/Global/StringTable/ST_BuildingDesc.ST_BuildingDesc'
	BuildDesc = FText::FromStringTable("/Game/Global/StringTable/ST_BuildingDesc", *BuildDesc.ToString());
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
