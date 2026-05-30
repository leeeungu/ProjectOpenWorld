#include "Pal/Widget/PalHpWidget_MonsterDefault.h"
#include "Components/TextBlock.h"
#include "Pal/Widget/Infomation/StatusBarWidget.h"
#include "Pal/Data/StatusData.h"

void UPalHpWidget_MonsterDefault::SetStatWidget(UStatComponent* StatCom)
{
	if (HpProgressBar)
	{
		if(StatCom)
			HpProgressBar->BindStatWidget(StatCom, EStatusType::HP);
		else
			HpProgressBar->UnBindStatWidget(StatCom, EStatusType::HP);
	}
}

void UPalHpWidget_MonsterDefault::InitializeHPWidget(FName PalID, int32 Level, double CurStat, double _MaxHP)
{
	if (MonsterNameTextBlock)
	{
		FText MonsterName = FText::FromString(PalID.ToString());
		MonsterName = FText::FromStringTable("/Game/Pal/StringTable/ST_PalName.ST_PalName", *MonsterName.ToString());
		MonsterNameTextBlock->SetText(MonsterName);
	}
	if (MonsterLevel)
	{
		FText LevelText = FText::FromString(TEXT("Lv.") + FString::FormatAsNumber(Level));
		MonsterLevel->SetText(LevelText);
	}
	if (HpProgressBar)
	{
		HpProgressBar->SetInitial(CurStat, _MaxHP);
	}
}