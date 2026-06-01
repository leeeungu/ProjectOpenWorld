#include "Pal/Widget/PalHpWidget_Boss.h"
#include "Components/TextBlock.h"
#include "Pal/Widget/Infomation/StatusBarWidget.h"
#include "Pal/Widget/Infomation/StatusTextBlock.h"
#include "GameBase/Component/StatComponent.h"

void UPalHpWidget_Boss::SetupInfo(FName DisplayName, int32 Level)
{
	if (MonsterNameTextBlock)
	{
		FText MonsterName = FText::FromString(DisplayName.ToString());
		//Script/Engine.StringTable'/Game/Pal/StringTable/ST_PalName.ST_PalName'
		MonsterName = FText::FromStringTable("/Game/Pal/StringTable/ST_PalName.ST_PalName", *MonsterName.ToString());
		MonsterNameTextBlock->SetText(MonsterName);
	}
	if (MonsterLevel)
	{
		FText LevelText = FText::FromString(TEXT("Lv.") + FString::FormatAsNumber(Level));
		MonsterLevel->SetText(LevelText);
	}
}


void UPalHpWidget_Boss::UnbindStat()
{
	if (StatComponent.IsValid() && HpProgressBar && MonsterHPText)
	{
		if (FOnStatChanged* CurrentDel = StatComponent->GetCurrentOnStatChanged(Stat))
		{
			CurrentDel->RemoveDynamic(HpProgressBar, &UStatusBarWidget::OnCurrentStatusChanged);
			CurrentDel->RemoveDynamic(MonsterHPText, &UStatusTextBlock::OnCurrentStatusChanged);
			
		}
		if (FOnStatChanged* MaxDel = StatComponent->GetMaxOnStatChanged(Stat))
		{
			MaxDel->RemoveDynamic(HpProgressBar, &UStatusBarWidget::OnMaxStatusChanged);
			MaxDel->RemoveDynamic(MonsterHPText, &UStatusTextBlock::OnMaxStatusChanged);
		}
	}
}

void UPalHpWidget_Boss::BindStat()
{
	if (StatComponent.IsValid() && HpProgressBar && MonsterHPText)
	{
		double CurrentStat = StatComponent->GetCurrentStat(Stat);
		double MaxStat = StatComponent->GetMaxStat(Stat);
		if (FOnStatChanged* CurrentDel = StatComponent->GetCurrentOnStatChanged(Stat))
		{
			CurrentDel->AddDynamic(HpProgressBar, &UStatusBarWidget::OnCurrentStatusChanged);
			CurrentDel->AddDynamic(MonsterHPText, &UStatusTextBlock::OnCurrentStatusChanged);
		}
		if (FOnStatChanged* MaxDel = StatComponent->GetMaxOnStatChanged(Stat))
		{
			MaxDel->AddDynamic(HpProgressBar, &UStatusBarWidget::OnMaxStatusChanged);
			MaxDel->AddDynamic(MonsterHPText, &UStatusTextBlock::OnMaxStatusChanged);
		}
		HpProgressBar->SetInitial(CurrentStat, MaxStat);
		MonsterHPText-> SetInitial(CurrentStat, MaxStat);
	}
}