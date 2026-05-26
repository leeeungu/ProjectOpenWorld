#include "Pal/Widget/PalHpWidget_MonsterDefault.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UPalHpWidget_MonsterDefault::OnHPChanged(double PreStat, double CurStat)
{
	if (!HpProgressBar )
		return;
	if (MaxHP > 0)
	{
		HpProgressBar->SetPercent(CurStat / MaxHP);
	}
	else
	{
		HpProgressBar->SetPercent(0);
	}
}

void UPalHpWidget_MonsterDefault::InitializeHPWidget(FName PalID, int32 Level, double CurStat, double _MaxHP)
{
	//OwnerMonster->OnDamagedDelegate.AddUniqueDynamic(this, &UPalHpWidget_MonsterDefault::OnDamageEvent);
	if (MonsterNameTextBlock)
	{
		FText MonsterName = FText::FromString(PalID.ToString());
		//Script/Engine.StringTable'/Game/Pal/StringTable/ST_PalName.ST_PalName'
		MonsterName = FText::FromStringTable("/Game/Pal/StringTable/ST_PalName.ST_PalName", *MonsterName.ToString());
		MonsterNameTextBlock->SetText(MonsterName);
	}
	if (MonsterLevel)
	{

		FText LevelText = FText::FromString(TEXT("Lv.") + FString::FormatAsNumber(Level));
		MonsterLevel->SetText(LevelText);
	}
	MaxHP = _MaxHP;
	if (!HpProgressBar)
		return;
	if (MaxHP > 0)
	{
		HpProgressBar->SetPercent(CurStat / MaxHP);
	}
	else
	{
		HpProgressBar->SetPercent(0);
	}
}