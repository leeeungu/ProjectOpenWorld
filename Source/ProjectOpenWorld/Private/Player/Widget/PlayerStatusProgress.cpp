#include "Player/Widget/PlayerStatusProgress.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UPlayerStatusProgress::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (StatusText)
	{
		StatusText->SetText(FText::FromString(TEXT("0")));
	}
	if (MaxStatusText)
	{
		MaxStatusText->SetText(FText::FromString(TEXT(" / 1")));
	}
	if (StatusProgress)
	{
		StatusProgress->SetFillColorAndOpacity(ProgressColor);
	}

	if (StatusImage)
	{
		StatusImage->SetBrushFromTexture(StatusTexture);
	}
	ABasePlayer* Player= GetOwningPlayerPawn<ABasePlayer>();
	if (Player)
	{
		SetStatusProgress(Player->GetStatusRef(StatusType), Player->GetStatusRef(MaxStatusType));
		UpdateStatus();
	}
}

void UPlayerStatusProgress::NativeConstruct()
{
	Super::NativeConstruct();
	ABasePlayer* Player = GetOwningPlayerPawn<ABasePlayer>();
	if (Player)
	{
		SetStatusProgress(Player->GetStatusRef(StatusType), Player->GetStatusRef(MaxStatusType));
		UpdateStatus();
	}
}

void UPlayerStatusProgress::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ABasePlayer* Player = GetOwningPlayerPawn<ABasePlayer>();
	if (Player)
	{
		SetStatusProgress(Player->GetStatusRef(StatusType), Player->GetStatusRef(MaxStatusType));
		UpdateStatus();
	}
}

void UPlayerStatusProgress::SetStatusProgress(float* Value, float* MaxValue)
{
	StatusRef = Value;
	MaxStatusRef = MaxValue;
}

void UPlayerStatusProgress::UpdateStatus()
{
	if (!StatusRef || !MaxStatusRef)
		return;

	if (StatusProgress)
	{
		StatusProgress->SetPercent(GetStatusPercent());
	}
	if (StatusText)
	{
		StatusText->SetText(FText::Format(FText::FromString(TEXT("{0}")), (int)*StatusRef));
	}
	if (MaxStatusText)
	{
		MaxStatusText->SetText(FText::Format(FText::FromString(TEXT(" / {0}")), (int)*MaxStatusRef));
	}
}

float UPlayerStatusProgress::GetStatusPercent() const
{
	if (StatusRef && MaxStatusRef)
		return *StatusRef / *MaxStatusRef;
	return 0.4f;
}
