#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PalHpWidget.generated.h"

UCLASS(Abstract)
class PROJECTOPENWORLD_API UPalHpWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION()
	virtual void OnHPChanged(double PreStat, double CurStat) PURE_VIRTUAL(UPalHpWidget::OnHPChanged, );
	virtual  void InitializeHPWidget(FName PalID, int32 Level, double CurStat, double _MaxHP)PURE_VIRTUAL (UPalHpWidget::Initialize, );
};
