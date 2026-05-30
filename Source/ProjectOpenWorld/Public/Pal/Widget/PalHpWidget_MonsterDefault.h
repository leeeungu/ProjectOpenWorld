#pragma once

#include "CoreMinimal.h"
#include "Pal/Widget/PalHpWidget.h"
#include "PalHpWidget_MonsterDefault.generated.h"

class UTextBlock;
class UStatusBarWidget;
class UStatComponent;

UCLASS()
class PROJECTOPENWORLD_API UPalHpWidget_MonsterDefault : public UPalHpWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UStatusBarWidget> HpProgressBar{};
	UPROPERTY(meta = (BindWidget))
	TObjectPtr <UTextBlock> MonsterNameTextBlock{};
	UPROPERTY(meta = (BindWidget))
	TObjectPtr < UTextBlock> MonsterLevel{};

	TWeakObjectPtr< UStatComponent> MonsterStatComponent{};
public:
	virtual void SetStatWidget(UStatComponent* StatCom) override;
	virtual void InitializeHPWidget(FName PalID, int32 Level, double CurStat, double _MaxHP) override;
};
