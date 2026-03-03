#include "Building/Widget/BuildingInfoWidget.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Building/Subsystem/BuildingDataSubsystem.h"
#include "Building/Widget/PalBuildingRecipeMaterial.h"
#include "GameBase/FunctionLib/StringTableFunctionLibrary.h"

void UBuildingInfoWidget::SetBuildingInfoData(FName InBuildObjectId)
{
	BuildObjectId = InBuildObjectId;
	if (!UBuildingDataSubsystem::IsValidInstance())
		return;
	const FPalBuildObjectData* BuildObjectData{};
	if (UBuildingDataSubsystem::GetPalBuildObjectData(BuildObjectId, BuildObjectData) && BuildObjectData)
	{
		if (BuildObjectIdText)
		{
			FText Text = FText::FromName(*BuildObjectData->MapObjectId);
			//Script/Engine.StringTable'/Game/Global/StringTable/ST_BuildingName.ST_BuildingName'
			Text = UStringTableFunctionLibrary::GetBuildingNameFromStringTable(*Text.ToString());
			BuildObjectIdText->SetText(Text);
		}
		if (MaterialBox)
		{
			int32 Count = MaterialBox->GetChildrenCount();
			for (int32 i = 0; i < Count; i++)
			{
				UPalBuildingRecipeMaterial* ChildWidget = Cast< UPalBuildingRecipeMaterial>(MaterialBox->GetChildAt(i));
				FName MaterialId = NAME_None;
				int32 MaterialCount = 0;
				if(BuildObjectData->Materials.IsValidIndex(i))
				{
					MaterialId = BuildObjectData->Materials[i].Material_Id;
					MaterialCount = BuildObjectData->Materials[i].Material_Count;
				}
				if (ChildWidget)
					ChildWidget->SetMaterialData(MaterialId, MaterialCount);
			}
		}
	}
}

void UBuildingInfoWidget::UpdateBuildingInfo()
{
	if (MaterialBox)
	{
		int32 Count = MaterialBox->GetChildrenCount();
		for (int32 i = 0; i < Count; i++)
		{
			UPalBuildingRecipeMaterial* ChildWidget = Cast< UPalBuildingRecipeMaterial>(MaterialBox->GetChildAt(i));
			if (ChildWidget)
				ChildWidget->UpdateMaterialData();
		}
	}
}
