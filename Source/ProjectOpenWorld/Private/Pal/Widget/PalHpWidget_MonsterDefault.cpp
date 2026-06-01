#include "Pal/Widget/PalHpWidget_MonsterDefault.h"
#include "Components/TextBlock.h"
#include "Pal/Widget/Infomation/StatusBarWidget.h"
#include "GameBase/Component/StatComponent.h"


void UPalHpWidget_MonsterDefault::SetupInfo(FName DisplayName, int32 Level)
{
	if (MonsterNameTextBlock)
	{
		FText MonsterName = FText::FromString(DisplayName.ToString());
		//Script/Engine.StringTable'/Game/Pal/StringTable/ST_PalName.ST_PalName'
		const FText NameText = FText::FromStringTable(TEXT("/Game/Pal/StringTable/ST_PalName.ST_PalName"), *MonsterName.ToString());
		MonsterNameTextBlock->SetText(NameText);
	}
	if (MonsterLevel)
	{
		MonsterLevel->SetText(FText::FromString(TEXT("Lv.") + FString::FromInt(Level)));
	}
}

void UPalHpWidget_MonsterDefault::UnbindStat()
{
	if (StatComponent.IsValid() && HpProgressBar)
	{
		if (FOnStatChanged* CurrentDel = StatComponent->GetCurrentOnStatChanged(Stat))
		{
			CurrentDel->RemoveDynamic(HpProgressBar, &UStatusBarWidget::OnCurrentStatusChanged);
		}
		if (FOnStatChanged* MaxDel = StatComponent->GetMaxOnStatChanged(Stat))
		{
			MaxDel->RemoveDynamic(HpProgressBar, &UStatusBarWidget::OnMaxStatusChanged);
		}
	}
}

void UPalHpWidget_MonsterDefault::BindStat()
{
	if (StatComponent.IsValid() && HpProgressBar)
	{
		double CurrentStat = StatComponent->GetCurrentStat(Stat);
		double MaxStat = StatComponent->GetMaxStat(Stat);
		if (FOnStatChanged* CurrentDel = StatComponent->GetCurrentOnStatChanged(Stat))
		{
			CurrentDel->AddDynamic(HpProgressBar, &UStatusBarWidget::OnCurrentStatusChanged);
		}
		if (FOnStatChanged* MaxDel = StatComponent->GetMaxOnStatChanged(Stat))
		{
			MaxDel->AddDynamic(HpProgressBar, &UStatusBarWidget::OnMaxStatusChanged);
		}
		HpProgressBar->SetInitial(CurrentStat, MaxStat);
	}
}
