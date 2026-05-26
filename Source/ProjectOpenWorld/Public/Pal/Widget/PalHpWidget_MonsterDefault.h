#pragma once

#include "CoreMinimal.h"
#include "Pal/Widget/PalHpWidget.h"
#include "PalHpWidget_MonsterDefault.generated.h"

class UTextBlock;
class UProgressBar;

UCLASS()
class PROJECTOPENWORLD_API UPalHpWidget_MonsterDefault : public UPalHpWidget
{
	GENERATED_BODY()
protected:
	double MaxHP{};
	double CurHP{};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HpProgressBar{};
	UPROPERTY(meta = (BindWidget))
	TObjectPtr <UTextBlock> MonsterNameTextBlock{};
	UPROPERTY(meta = (BindWidget))
	TObjectPtr < UTextBlock> MonsterLevel{};

public:
	virtual void OnHPChanged(double PreStat, double CurStat) override;
	virtual void InitializeHPWidget(FName PalID, int32 Level, double CurStat, double _MaxHP) override;
};
