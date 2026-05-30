#include "Pal/Widget/Infomation/StatusTextBlock.h"
#include "GameBase/Component/StatComponent.h"

void UStatusTextBlock::OnCurrentStatusChanged(double PreCurrentStat, double CurrentStat)
{
	SetInitial(CurrentStat, Max);
}

void UStatusTextBlock::OnMaxStatusChanged(double PreMaxStat, double MaxStat)
{
	SetInitial(Cur, MaxStat);
}

void UStatusTextBlock::SetInitial(double CurrentStat, double MaxStat)
{
	if (CurrentStat < 0.0 || MaxStat < 0.0)
		return;
	Cur = CurrentStat;
	Max = MaxStat;
	FText Result = FText::Format(FText::FromString(TEXT("{0} / {1}")), (int)Cur, (int)Max);
	SetText(Result);
}

void UStatusTextBlock::BindStatWidget(UStatComponent* StatCom, EStatusType StatusType)
{
	if (!StatCom)
		return;
	StatCom->GetCurrentOnStatChanged(StatusType)->AddUniqueDynamic(this, &UStatusTextBlock::OnCurrentStatusChanged);
	StatCom->GetMaxOnStatChanged(StatusType)->AddUniqueDynamic(this, &UStatusTextBlock::OnMaxStatusChanged);
}

void UStatusTextBlock::UnBindStatWidget(UStatComponent* StatCom, EStatusType StatusType)
{
	if (!StatCom)
		return;
	StatCom->GetCurrentOnStatChanged(StatusType)->RemoveDynamic(this, &UStatusTextBlock::OnCurrentStatusChanged);
	StatCom->GetMaxOnStatChanged(StatusType)->RemoveDynamic(this, &UStatusTextBlock::OnMaxStatusChanged);
}