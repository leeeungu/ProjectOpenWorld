#include "Player/Widget/PlayerStatusProgress.h"
#include "Components/Image.h"
#include "GameBase/Component/StatComponent.h"
#include "Pal/Widget/Infomation/StatusBarWidget.h"
#include "Pal/Widget/Infomation/StatusTextBlock.h"

void UPlayerStatusProgress::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (StatusImage)
	{
		StatusImage->SetBrushFromTexture(StatusTexture);
	}
	if (StatusBarWidget)
	{
		StatusBarWidget->SetColor(0, ProgressColor);
		FLinearColor BackProgressColor = ProgressColor;
		BackProgressColor.A = 0.3;
		StatusBarWidget->SetColor(1, BackProgressColor);
	}
}

void UPlayerStatusProgress::NativeConstruct()
{
	Super::NativeConstruct();
	BindStat();
}

void UPlayerStatusProgress::NativeDestruct()
{
	Super::NativeDestruct();
	UnBindStat();
}

void UPlayerStatusProgress::SetStatWidget(UStatComponent* StatCom)
{
	PlayerStatCom = StatCom;
	BindStat();
}

void UPlayerStatusProgress::BindStat()
{
	if (PlayerStatCom.IsValid() && StatusType != EStatusType::None)
	{
		if (StatusBarWidget)
		{
			StatusBarWidget->SetInitial(PlayerStatCom->GetCurrentStat(StatusType), PlayerStatCom->GetMaxStat(StatusType));
			StatusBarWidget->BindStatWidget(PlayerStatCom.Get(), StatusType);
		}

		if (StatusText)
		{
			StatusText->SetInitial(PlayerStatCom->GetCurrentStat(StatusType), PlayerStatCom->GetMaxStat(StatusType));
			StatusText->BindStatWidget(PlayerStatCom.Get(), StatusType);
		}
	}
}

void UPlayerStatusProgress::UnBindStat()
{
	if (PlayerStatCom.IsValid() && StatusType != EStatusType::None)
	{
		if (StatusBarWidget)
		{
			StatusBarWidget->UnBindStatWidget(PlayerStatCom.Get(), StatusType);
		}

		if (StatusText)
		{
			StatusText->UnBindStatWidget(PlayerStatCom.Get(), StatusType);
		}
	}
}
