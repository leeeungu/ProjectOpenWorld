#include "Player/Widget/MainUI.h"
#include "Item/Widget/WeaponeWidget.h"
#include "Player/Widget/PlayerStatusProgress.h"

void UMainUI::ChangeWeapone(FName NewWeaponeID, EWeapone NewWeaponeType)
{
	if (WBP_WeaponUI)
	{
		WBP_WeaponUI->ChangeWeapone(NewWeaponeID, NewWeaponeType);
	}
}

void UMainUI::SetStatWidget(UStatComponent* StatCom)
{
	if (PlayerHPBar)
	{
		PlayerHPBar->SetStatWidget(StatCom);
	}
}

void UMainUI::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMainUI::NativeDestruct()
{
	Super::NativeDestruct();
}