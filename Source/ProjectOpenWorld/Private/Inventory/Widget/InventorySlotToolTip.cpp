#include "Inventory/Widget/InventorySlotToolTip.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Item/DataTable/PalStaticItemDataStruct.h"


void UInventorySlotToolTip::NativeConstruct()
{
	Super::NativeConstruct();
}

void UInventorySlotToolTip::SetRecipeID(FName InRecipeID)
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
		if (ItemNameText)
		{
			FText ItemName = FText::FromString(
				Result->OverrideName == "None" ? RecipeID.ToString() :
				Result->OverrideName
			);
			//Begin Object Class = / Script / BlueprintGraph.K2Node_CallFunction Name = "K2Node_CallFunction_0" ExportPath = "/Script/BlueprintGraph.K2Node_CallFunction'/Game/Player/Widget/Status/WBP_StatusWidget.WBP_StatusWidget:EventGraph.K2Node_CallFunction_0'"
			//	bIsPureFunc = True
			//	FunctionReference = (MemberParent = "/Script/CoreUObject.Class'/Script/Engine.KismetSystemLibrary'", MemberName = "MakeLiteralText")
			//	NodePosX = 464
			//	NodePosY = 288
			//	NodeGuid = BE5DAED34342E76BC934388D0D91D29C
			//	CustomProperties Pin(PinId = A5B5F288479ACE2E01ED40A4E2F31375, PinName = "self", PinFriendlyName = NSLOCTEXT("K2Node", "Target", "Target"), PinToolTip = "Target\nKismet System Library 오브젝트 레퍼런스", PinType.PinCategory = "object", PinType.PinSubCategory = "", PinType.PinSubCategoryObject = "/Script/CoreUObject.Class'/Script/Engine.KismetSystemLibrary'", PinType.PinSubCategoryMemberReference = (), PinType.PinValueType = (), PinType.ContainerType = None, PinType.bIsReference = False, PinType.bIsConst = False, PinType.bIsWeakPointer = False, PinType.bIsUObjectWrapper = False, PinType.bSerializeAsSinglePrecisionFloat = False, DefaultObject = "/Script/Engine.Default__KismetSystemLibrary", PersistentGuid = 00000000000000000000000000000000, bHidden = True, bNotConnectable = False, bDefaultValueIsReadOnly = False, bDefaultValueIsIgnored = False, bAdvancedView = False, bOrphanedPin = False, )
			//	CustomProperties Pin(PinId = 25FC937D43C0C9B88E9BEA83C1F8904A, PinName = "Value", PinToolTip = "Value\n텍스트\n\n값 - 설정할 FText (포맷 텍스트) 값입니다.", PinType.PinCategory = "text", PinType.PinSubCategory = "", PinType.PinSubCategoryObject = None, PinType.PinSubCategoryMemberReference = (), PinType.PinValueType = (), PinType.ContainerType = None, PinType.bIsReference = False, PinType.bIsConst = False, PinType.bIsWeakPointer = False, PinType.bIsUObjectWrapper = False, PinType.bSerializeAsSinglePrecisionFloat = False, DefaultTextValue = LOCTABLE("/Game/Global/StringTable/ST_PSN_EULA.ST_PSN_EULA", "Wood"), PersistentGuid = 00000000000000000000000000000000, bHidden = False, bNotConnectable = False, bDefaultValueIsReadOnly = False, bDefaultValueIsIgnored = False, bAdvancedView = False, bOrphanedPin = False, )
			//	CustomProperties Pin(PinId = 258E2CAD41BD3D9E897FE19C56AF85BC, PinName = "ReturnValue", PinToolTip = "Return Value\n텍스트\n\n리터럴 FText (포맷 텍스트)", Direction = "EGPD_Output", PinType.PinCategory = "text", PinType.PinSubCategory = "", PinType.PinSubCategoryObject = None, PinType.PinSubCategoryMemberReference = (), PinType.PinValueType = (), PinType.ContainerType = None, PinType.bIsReference = False, PinType.bIsConst = False, PinType.bIsWeakPointer = False, PinType.bIsUObjectWrapper = False, PinType.bSerializeAsSinglePrecisionFloat = False, PersistentGuid = 00000000000000000000000000000000, bHidden = False, bNotConnectable = False, bDefaultValueIsReadOnly = False, bDefaultValueIsIgnored = False, bAdvancedView = False, bOrphanedPin = False, )
			//	End Object
			ItemName = FText::FromStringTable("/Game/Global/StringTable/ST_PSN_EULA", *ItemName.ToString());
			//ItemName.AsCultureInvariant
			ItemNameText->SetText(ItemName);
		}
		if (ItemIconImage)
		{
			UTexture2D* IconTexture = UItemDataSubsystem::GetPalItemIconTextureByName(*Result->IconName);
			if (IconTexture)
			{
				ItemIconImage->SetBrushFromTexture(IconTexture);
			}
		}
		if (ItemDescText)
		{
			FText Desc = FText::FromString(
				Result->OverrideName == "None" ? RecipeID.ToString() :
				Result->OverrideName
			);
			Desc = FText::FromStringTable("/Game/Global/StringTable/ST_PalItemDesc", *Desc.ToString());
			ItemDescText->SetText(Desc);
		}
	}
}
