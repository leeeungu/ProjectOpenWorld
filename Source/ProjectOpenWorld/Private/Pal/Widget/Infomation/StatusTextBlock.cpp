#include "Pal/Widget/Infomation/StatusTextBlock.h"

void UStatusTextBlock::OnCurrentStatusChanged(double PreCurrentStat, double CurrentStat)
{
	Cur = CurrentStat;
	SetInitial(Cur, Max);
}

void UStatusTextBlock::OnMaxStatusChanged(double PreMaxStat, double MaxStat)
{
	Max = MaxStat;
	SetInitial(Cur, Max);
}

void UStatusTextBlock::SetInitial(double CurrentStat, double MaxStat)
{
	FText Result = FText::Format(FText::FromString(TEXT("{0} / {1}")), (int)CurrentStat, (int)MaxStat);
	SetText(Result);
}