// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pal/Widget/PalHpWidget.h"
#include "PalHpWidget_Boss.generated.h"

class UTextBlock;
class UProgressBar;

UCLASS()
class PROJECTOPENWORLD_API UPalHpWidget_Boss : public UPalHpWidget
{
	GENERATED_BODY()

protected:
	double MaxHP{};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HpProgressBar{};
	UPROPERTY(meta = (BindWidget))
	TObjectPtr <UTextBlock> MonsterNameTextBlock{};
	UPROPERTY(meta = (BindWidget))
	TObjectPtr < UTextBlock> MonsterLevel{};
	UPROPERTY(meta = (BindWidget))
	TObjectPtr < UTextBlock> MonsterHPText{};
public:
	void OnHPChanged(double PreStat, double CurStat) ;

	virtual void SetStatWidget(UStatComponent* StatCom) {}
	virtual void InitializeHPWidget(FName PalID, int32 Level, double CurStat, double _MaxHP) override;
};
