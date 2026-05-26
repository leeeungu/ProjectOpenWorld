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
			PlayerStatCom->GetCurrentOnStatChanged(StatusType)->AddUniqueDynamic(StatusBarWidget, &UStatusBarWidget::OnCurrentStatusChanged);
			PlayerStatCom->GetMaxOnStatChanged(StatusType)->AddUniqueDynamic(StatusBarWidget, &UStatusBarWidget::OnMaxStatusChanged);
			StatusBarWidget->SetInitial(PlayerStatCom->GetCurrentStat(StatusType), PlayerStatCom->GetMaxStat(StatusType));
		}

		if (StatusText)
		{
			PlayerStatCom->GetCurrentOnStatChanged(StatusType)->AddUniqueDynamic(StatusText, &UStatusTextBlock::OnCurrentStatusChanged);
			PlayerStatCom->GetMaxOnStatChanged(StatusType)->AddUniqueDynamic(StatusText, &UStatusTextBlock::OnMaxStatusChanged);
			StatusText->SetInitial(PlayerStatCom->GetCurrentStat(StatusType), PlayerStatCom->GetMaxStat(StatusType));
		}
	}
}

void UPlayerStatusProgress::UnBindStat()
{
	if (PlayerStatCom.IsValid() && StatusType != EStatusType::None)
	{
		if (StatusBarWidget)
		{
			PlayerStatCom->GetCurrentOnStatChanged(StatusType)->RemoveDynamic(StatusBarWidget, &UStatusBarWidget::OnCurrentStatusChanged);
			PlayerStatCom->GetMaxOnStatChanged(StatusType)->RemoveDynamic(StatusBarWidget, &UStatusBarWidget::OnMaxStatusChanged);
		}

		if (StatusText)
		{
			PlayerStatCom->GetCurrentOnStatChanged(StatusType)->RemoveDynamic(StatusText, &UStatusTextBlock::OnCurrentStatusChanged);
			PlayerStatCom->GetMaxOnStatChanged(StatusType)->RemoveDynamic(StatusText, &UStatusTextBlock::OnMaxStatusChanged);
		}
	}
}
