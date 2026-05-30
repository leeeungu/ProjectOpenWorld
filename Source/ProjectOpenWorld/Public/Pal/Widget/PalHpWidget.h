#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PalHpWidget.generated.h"

class UStatComponent;

UCLASS(Abstract)
class PROJECTOPENWORLD_API UPalHpWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION()
	virtual void SetStatWidget(UStatComponent* StatCom)PURE_VIRTUAL(UPalHpWidget::SetStatWidget, );
	UFUNCTION()
	virtual void InitializeHPWidget(FName PalID, int32 Level, double CurStat, double _MaxHP) PURE_VIRTUAL (UPalHpWidget::Initialize, );
};
