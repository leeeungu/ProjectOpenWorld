#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StatusBarWidget.generated.h"

class UProgressBar;
class UOverlay;

UCLASS(BlueprintType)
class PROJECTOPENWORLD_API UStatusBarWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetColor(int32 Index, FLinearColor Color);
    void SetInitial(double InCurrent, double InMax);

    UFUNCTION()
    void OnCurrentStatusChanged(double PreCurrentStat, double CurrentStat);

    UFUNCTION()
    void OnMaxStatusChanged(double PreMaxStat, double InMaxStat);

protected:
    /** 인덱스 0이 가장 먼저 소모. 순서가 곧 디자인 */
    UPROPERTY(EditAnywhere, Category = "Status|Config")
    TArray<FLinearColor> SegmentColors = {
        FLinearColor(1.0f, 0.0f, 0.0f),
        FLinearColor(1.0f, 0.5f, 0.0f),
        FLinearColor(1.0f, 1.0f, 0.0f),
        FLinearColor(0.0f, 0.8f, 0.0f),
    };

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status|Config", meta = (ClampMin = "0.05", ClampMax = "5.0"))
    float TransitionDuration = 0.3f;

    UPROPERTY(meta = (BindWidget) )
    TObjectPtr<UOverlay> BarOverlay{};

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> ActiveBar{};   // top: 소모 중
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> PreviewBar{};  // bottom: 다음 색 100%

    UPROPERTY(EditAnywhere, Category = "Status|Bar")
    bool bSwapColors = true;
protected:
    virtual void NativeConstruct() override;
    virtual void NativePreConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
private:
    void    InitBars();
    void    SwapColors(int32 SegIdx);            // Property swap만, 구조 변경 없음
    void    BeginTransition(double InStat, bool bFromFull);
    int32   CalcSegIdx(double Stat) const;
    float   CalcIntraRatio(double Stat, int32 SegIdx) const;
    FLinearColor SafeColor(int32 SegIdx) const;

    // 2개만 사용. 구조 고정, 절대 재배치 안 함

    double MaxStat = 1.0;
    double CurrentStat = 1.0;
    int32  SegIdx = 0;

    float  DisplayRatio = 1.0f;
    float  StartRatio = 1.0f;
    float  TargetRatio = 1.0f;
    float  Elapsed = 0.0f;
    bool   bTicking = false;

#if WITH_EDITORONLY_DATA
    UPROPERTY(EditAnywhere, Category = "HP|Debug")
    bool bDebugLog = false;
#endif
};