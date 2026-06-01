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
protected:
	virtual void SetupInfo(FName DisplayName, int32 Level) override;
	virtual void UnbindStat() override;
	virtual void BindStat() override;
};
