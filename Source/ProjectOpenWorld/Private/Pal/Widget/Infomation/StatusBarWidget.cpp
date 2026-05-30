#include "Pal/Widget/Infomation/StatusBarWidget.h"
#include "Components/ProgressBar.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "GameBase/Component/StatComponent.h"

// ── 라이프사이클 ─────────────────────────────────────────────────────────────

void UStatusBarWidget::NativeConstruct()
{
    Super::NativeConstruct();
    InitBars();
}

void UStatusBarWidget::NativePreConstruct()
{
    Super::NativeConstruct();
    InitBars();
}

void UStatusBarWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    if (!bTicking) 
        return;

    Elapsed += InDeltaTime;
    const float Alpha = FMath::Clamp(Elapsed / TransitionDuration, 0.0f, 1.0f);
    DisplayRatio = FMath::Lerp(StartRatio, TargetRatio, Alpha);

    if (ActiveBar)
    {
        ActiveBar->SetPercent(DisplayRatio);   // Paint 무효화만 발생
    }

    if (Alpha >= 1.0f)
    {
        DisplayRatio = TargetRatio;
        bTicking = false;
    }
}

// ── 외부 진입점 ──────────────────────────────────────────────────────────────

void UStatusBarWidget::SetColor(int32 Index, FLinearColor Color)
{
    if (SegmentColors.IsValidIndex(Index))
    {
        SegmentColors[Index] = Color;
    }
    InitBars();
}

void UStatusBarWidget::SetInitial(double InCurrent, double InMax)
{
    MaxStat = FMath::Max(InMax, 1.0);
    CurrentStat = FMath::Clamp(InCurrent, 0.0, MaxStat);
    SegIdx = CalcSegIdx(CurrentStat);

    SwapColors(SegIdx);

    DisplayRatio = StartRatio = TargetRatio = CalcIntraRatio(CurrentStat, SegIdx);
    if(ActiveBar)
        ActiveBar->SetPercent(DisplayRatio);
    bTicking = false;
}

void UStatusBarWidget::OnCurrentStatusChanged(double PreCurrentStat, double InCurrentStat)
{
    const double Prev = InCurrentStat;
    CurrentStat = FMath::Clamp(InCurrentStat, 0.0, MaxStat);

    const int32 NewSegIdx = CalcSegIdx(CurrentStat);
    const bool bSegChanged = (NewSegIdx != SegIdx);
    const bool bIsDamage = (CurrentStat < PreCurrentStat);

    if (bSegChanged)
    {
        SegIdx = NewSegIdx;
        SwapColors(SegIdx);          // Layout 무효화 없음
    }

    BeginTransition(CurrentStat, bSegChanged && bIsDamage);
    /*
#if WITH_EDITOR
    if (bDebugLog && GEngine)
    {
        const double  Delta = CurrentStat - PreCurrentStat;
        const FString Msg = FString::Printf(
            TEXT("[HPBar] %.1f→%.1f/%.1f  (%s%.1f)  Seg:%d  %.2f"),
            PreCurrentStat, CurrentStat, MaxStat,
            Delta >= 0.0 ? TEXT("+") : TEXT(""), Delta,
            SegIdx, TargetRatio);

        UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);
        GEngine->AddOnScreenDebugMessage(-1, 2.0f,
            bIsDamage ? FColor::Red : FColor::Green, Msg);
    }
#endif
    */
}

void UStatusBarWidget::OnMaxStatusChanged(double PreMaxStat, double InMaxStat)
{
    SetInitial(CurrentStat, InMaxStat);
}

void UStatusBarWidget::BindStatWidget(UStatComponent* StatCom, EStatusType StatusType)
{
    if (!StatCom)
        return;
    StatCom->GetCurrentOnStatChanged(StatusType)->AddUniqueDynamic(this, &UStatusBarWidget::OnCurrentStatusChanged);
    StatCom->GetMaxOnStatChanged(StatusType)->AddUniqueDynamic(this, &UStatusBarWidget::OnMaxStatusChanged);
}

void UStatusBarWidget::UnBindStatWidget(UStatComponent* StatCom, EStatusType StatusType)
{
    if (!StatCom)
        return;
    StatCom->GetCurrentOnStatChanged(StatusType)->RemoveDynamic(this, &UStatusBarWidget::OnCurrentStatusChanged);
    StatCom->GetMaxOnStatChanged(StatusType)->RemoveDynamic(this, &UStatusBarWidget::OnMaxStatusChanged);
}

// ── 내부 ─────────────────────────────────────────────────────────────────────

void UStatusBarWidget::InitBars()
{
    if (!BarOverlay)
        return;
    BarOverlay->ClearChildren();

    auto AddFill = [this](UProgressBar* Bar) {
        if (Bar)
            {
                Bar->WidgetStyle.BackgroundImage.TintColor = FSlateColor(FLinearColor(0.f, 0.f, 0.f, 0.f));
                if (UOverlaySlot* Slot = BarOverlay->AddChildToOverlay(Bar))
                {
                    Slot->SetHorizontalAlignment(HAlign_Fill);
                    Slot->SetVerticalAlignment(VAlign_Fill);
                }
            }
        };

    AddFill(PreviewBar);  
    AddFill(ActiveBar);   

    if (ActiveBar)
        ActiveBar->SetFillColorAndOpacity(SafeColor(0));
    if(PreviewBar)
        PreviewBar->SetFillColorAndOpacity(SafeColor(1));
}

void UStatusBarWidget::SwapColors(int32 InSegIdx)
{
    // 두 줄의 SetFillColorAndOpacity → Paint 무효화만, Layout 무효화 없음
    if (!ActiveBar || !PreviewBar || !bSwapColors)
        return;
    ActiveBar->SetFillColorAndOpacity(SafeColor(InSegIdx));
    PreviewBar->SetFillColorAndOpacity(SafeColor(InSegIdx + 1));
    PreviewBar->SetPercent(1.0f);
}

void UStatusBarWidget::BeginTransition(double InStat, bool bFromFull)
{
    if (bFromFull)
        DisplayRatio = 1.0f;
    StartRatio = DisplayRatio;
    TargetRatio = CalcIntraRatio(InStat, SegIdx);
    Elapsed = 0.0f;
    bTicking = true;
}

FLinearColor UStatusBarWidget::SafeColor(int32 Idx) const
{
    if (SegmentColors.IsEmpty())
        return FLinearColor::White;
    return SegmentColors[FMath::Abs(Idx) % SegmentColors.Num()];
}

int32 UStatusBarWidget::CalcSegIdx(double Stat) const
{
    int32 N = 0;
    if (bSwapColors)
        N = SegmentColors.Num();
    if (N == 0 || MaxStat <= 0.0)
        return 0;
    return FMath::Clamp(FMath::FloorToInt((MaxStat - Stat) / (MaxStat / N)), 0, N - 1);
}

float UStatusBarWidget::CalcIntraRatio(double Stat, int32 Idx) const
{
    int32 N = 1;
    if (bSwapColors)
        N = SegmentColors.Num();
    if (N == 0 || MaxStat <= 0.0)
        return 1.0f;
    const double PerSeg = MaxStat / N;
    const double SegMin = MaxStat - (Idx + 1) * PerSeg;
    return FMath::Clamp(static_cast<float>((Stat - SegMin) / PerSeg), 0.0f, 1.0f);
}