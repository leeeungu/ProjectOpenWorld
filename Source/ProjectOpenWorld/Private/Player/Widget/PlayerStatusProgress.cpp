#include "Player/Widget/PlayerStatusProgress.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Player/Character/BasePlayer.h"
#include "GameBase/Component/StatComponent.h"
#include "Components/Image.h"

void UPlayerStatusProgress::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (StatusText)
	{
		StatusText->SetText(GetStatusText());
	}
	if (MaxStatusText)
	{
		MaxStatusText->SetText(GetMaxStatusText());
	}
	if (StatusProgress)
	{
		StatusProgress->SetFillColorAndOpacity(ProgressColor);
		StatusProgress->SetPercent(GetStatusPercent());
	}

	if (StatusImage)
	{
		StatusImage->SetBrushFromTexture(StatusTexture);
	}
	//ABasePlayer* Player= GetOwningPlayerPawn<ABasePlayer>();
	//if (Player)
	//{
	//	//SetStatusProgress(Player->GetStatusRef(StatusType), Player->GetStatusRef(MaxStatusType));
	//	//UpdateStatus();
	//}
}

void UPlayerStatusProgress::NativeConstruct()
{
	Super::NativeConstruct();
	if (ABasePlayer* Player = GetOwningPlayerPawn<ABasePlayer>())
	{
		switch (StatusType)
		{
		case EStatusType::None:
			break;
		case EStatusType::Hp:
		{
			StatComponent = Player->GetHPStat();
			break;
		}
		case EStatusType::MaxHp:
		{
			StatComponent = Player->GetHPStat();
			break;
		}
		case EStatusType::Shield:
			break;
		case EStatusType::MaxShield:
			break;
		case EStatusType::Health:
			break;
		case EStatusType::MaxHealth:
			break;
		case EStatusType::Stamina:
			break;
		case EStatusType::Attack:
		{
			StatComponent = Player->GetAttackStat();
			break;
		}
		case EStatusType::Defense:
		{
			StatComponent = Player->GetDefendStat();
			break;
		}
		case EStatusType::WorkSpeed:
			break;
		case EStatusType::MaxWeight:
			break;
		case EStatusType::EnumMax:
			break;
		default:
		{
			break;
		}
		}
		if (StatComponent)
		{
			StatComponent->OnStatChanged.AddUniqueDynamic(this, &UPlayerStatusProgress::UpdateStatusData);
		}
		//SetStatusProgress(Player->GetStatusRef(StatusType), Player->GetStatusRef(MaxStatusType));
	}
	UpdateStatus();
}

void UPlayerStatusProgress::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UPlayerStatusProgress::UpdateStatusData(double PreCurrentValue, double PreMaxValue)
{
	UpdateStatus();
}

void UPlayerStatusProgress::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ABasePlayer* Player = GetOwningPlayerPawn<ABasePlayer>();
	if (Player)
	{
		//SetStatusProgress(Player->GetStatusRef(StatusType), Player->GetStatusRef(MaxStatusType));
		UpdateStatus();
	}
}

//void UPlayerStatusProgress::SetStatusProgress(float* Value, float* MaxValue)
//{
//	StatusRef = Value;
//	MaxStatusRef = MaxValue;
//}

void UPlayerStatusProgress::UpdateStatus()
{
	if (!StatComponent)
		return;

	if (StatusProgress)
	{
		StatusProgress->SetPercent(GetStatusPercent());
	}
	if (StatusText)
	{
		StatusText->SetText(GetStatusText());
	}
	if (MaxStatusText)
	{
		MaxStatusText->SetText(GetMaxStatusText());
	}
}

FText UPlayerStatusProgress::GetStatusText() const
{
	if (!StatComponent)
	{
		return FText::FromString(TEXT("0"));
	}
	return FText::Format(FText::FromString(TEXT("{0}")), (int)StatComponent->GetCurrentStat());
}

FText UPlayerStatusProgress::GetMaxStatusText() const
{
	if (!StatComponent)
	{
		return FText::FromString(TEXT(" / 1"));
	}
	return FText::Format(FText::FromString(TEXT(" / {0}")), (int)StatComponent->GetMaxStat());
}

float UPlayerStatusProgress::GetStatusPercent() const
{
	//	UE_LOG(LogTemp, Warning, TEXT("GetStatusPercent : %f / %f"), StatusRef ? *StatusRef : 0.0f, MaxStatusRef ? *MaxStatusRef : 0.0f);
	if (StatComponent)
		return StatComponent->GetStatPercent();
	return 0.1f;
}
