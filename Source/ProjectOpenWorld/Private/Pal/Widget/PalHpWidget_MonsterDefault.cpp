#include "Pal/Widget/PalHpWidget_MonsterDefault.h"
#include "Components/TextBlock.h"
#include "Pal/Widget/Infomation/StatusBarWidget.h"
#include "GameBase/Component/StatComponent.h"


void UPalHpWidget_MonsterDefault::SetupInfo(FName DisplayName, int32 Level)
{
	if (MonsterNameTextBlock)
	{
		// 기존 코드와 동일하게 StringTable로 표시명 변환
		const FText NameText = FText::FromStringTable(
			TEXT("/Game/Pal/StringTable/ST_PalName.ST_PalName"), DisplayName.ToString());
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
		HpProgressBar->SetInitial(CurrentStat, MaxStat);
		if (FOnStatChanged* CurrentDel = StatComponent->GetCurrentOnStatChanged(Stat))
		{
			CurrentDel->AddDynamic(HpProgressBar, &UStatusBarWidget::OnCurrentStatusChanged);
		}
		if (FOnStatChanged* MaxDel = StatComponent->GetMaxOnStatChanged(Stat))
		{
			MaxDel->AddDynamic(HpProgressBar, &UStatusBarWidget::OnMaxStatusChanged);
		}
	}
}
