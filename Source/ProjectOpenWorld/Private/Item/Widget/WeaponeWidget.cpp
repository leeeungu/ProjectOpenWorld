#include "Item/Widget/WeaponeWidget.h"
#include "GameBase/FunctionLib/StringTableFunctionLibrary.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UWeaponeWidget::ChangeWeapone(FName NewWeaponeID)
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
}
