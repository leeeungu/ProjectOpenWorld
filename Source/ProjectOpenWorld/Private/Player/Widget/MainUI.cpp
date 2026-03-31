#include "Player/Widget/MainUI.h"
#include "Item/Widget/WeaponeWidget.h"


void UMainUI::ChangeWeapone(FName NewWeaponeID, EWeapone NewWeaponeType)
{
	if (WBP_WeaponUI)
	{
		WBP_WeaponUI->ChangeWeapone(NewWeaponeID, NewWeaponeType);
	}
}

