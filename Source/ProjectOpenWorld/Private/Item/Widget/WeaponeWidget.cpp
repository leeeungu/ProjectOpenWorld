#include "Item/Widget/WeaponeWidget.h"
#include "GameBase/FunctionLib/StringTableFunctionLibrary.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Item/DataTable/WeaponeData.h"
#include "Item/System/ItemDataSubsystem.h"

void UWeaponeWidget::ChangeWeapone(FName NewWeaponeID, EWeapone NewWeaponeType)
{
	FText Result{};
	if (UStringTableFunctionLibrary::GetWeaponeNameFromStringTable(NewWeaponeID.ToString(), Result))
	{
		NameText->SetText(Result);
	}
	else // ÀåÂø ºÒ°¡
	{
		//NameText->SetText(FText::FromStringTable(UStringTableFunctionLibrary::GetWeaponeNameStringTable(), UStringTableFunctionLibrary::GetUnValidItem()));
	}

	if (WeaponImage)
	{
		UTexture2D* WeaponeUITexture = UItemDataSubsystem::GetWeaponUI(NewWeaponeType);
		WeaponImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		if (!WeaponeUITexture)
		{
			WeaponImage->SetVisibility(ESlateVisibility::Hidden);
		}
		WeaponImage->SetBrushFromTexture(WeaponeUITexture);

	}
}

void UWeaponeWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ChangeWeapone(FName(""), EWeapone::None);
}
