#include "Player/Widget/MainUI.h"
#include "Item/Widget/WeaponeWidget.h"
#include "Player/Widget/PlayerStatusProgress.h"
#include "Pal/Widget/MonsterHpLayer.h"

void UMainUI::ChangeWeapone(FName NewWeaponeID, EWeapone NewWeaponeType)
{
	if (WBP_WeaponUI)
	{
		WBP_WeaponUI->ChangeWeapone(NewWeaponeID, NewWeaponeType);
	}
}

void UMainUI::SetPlayerStatWidget(UStatComponent* StatCom)
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

void UMainUI::RegisterMonster(AActor* Target)
{
	if(!MonsterHpLayer)
		return;
	MonsterHpLayer->RegisterMonster(Target);
}

void UMainUI::UnregisterMonster(AActor* Target)
{
	if (!MonsterHpLayer)
		return;
	MonsterHpLayer->UnregisterMonster(Target);
}