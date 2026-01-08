#include "Player/Widget/PlayerStatusProgress.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
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

void UPlayerStatusProgress::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	UpdateStatus();
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
		StatusText->SetText(GetStatusText());
	}
	if (MaxStatusText)
	{
		MaxStatusText->SetText(GetMaxStatusText());
	}
}


FText UPlayerStatusProgress::GetStatusText() const
{
	if (!StatusRef)
	{
		return FText::FromString(TEXT("0"));
	}
	return FText::Format(FText::FromString(TEXT("{0}")), (int)*StatusRef);
}

FText UPlayerStatusProgress::GetMaxStatusText() const
{
	if (!MaxStatusRef)
	{
		return FText::FromString(TEXT(" / 1"));
	}
	return FText::Format(FText::FromString(TEXT(" / {0}")), (int)*MaxStatusRef);
}

float UPlayerStatusProgress::GetStatusPercent() const
{
//	UE_LOG(LogTemp, Warning, TEXT("GetStatusPercent : %f / %f"), StatusRef ? *StatusRef : 0.0f, MaxStatusRef ? *MaxStatusRef : 0.0f);
	if (StatusRef && MaxStatusRef)
		return *StatusRef / *MaxStatusRef;
	return 0.1f;
}
