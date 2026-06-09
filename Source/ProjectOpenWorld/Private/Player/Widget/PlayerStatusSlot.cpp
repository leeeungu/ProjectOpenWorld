#include "Player/Widget/PlayerStatusSlot.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "GameBase/Component/StatComponent.h"

void UPlayerStatusSlot::NativePreConstruct()
{
	UUserWidget::NativePreConstruct();
	if (StatusNameText)
	{
		StatusNameText->SetText(StatusName);
	}
	if (StatusImage)
	{
		StatusImage->SetBrushFromTexture(StatusTexture);
	}
}

void UPlayerStatusSlot::OnMaxStatusChanged(double PreMaxStat, double InMaxStat)
{
	if (StatusText)
	{
		StatusText->SetText(FText::Format(FText::FromString(TEXT("{0}")), (int)InMaxStat));
	}
}

void UPlayerStatusSlot::SetStatWidget(UStatComponent* StatCom)
{
	if (StatCom)
	{
		double CurrentStat = StatCom->GetCurrentStat(StatusType);
		double MaxStat = StatCom->GetMaxStat(StatusType);
		OnMaxStatusChanged(0, MaxStat);
		if (FOnStatChanged* CurrentDel = StatCom->GetMaxOnStatChanged(StatusType))
		{
			CurrentDel->AddDynamic(this, &UPlayerStatusSlot::OnMaxStatusChanged);
		}
		//HpProgressBar->SetInitial(CurrentStat, MaxStat);
		//MonsterHPText->SetInitial(CurrentStat, MaxStat);
	}
}