// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pal/Widget/PalHpWidget.h"
#include "PalHpWidget_Boss.generated.h"

class UTextBlock;
class UStatusBarWidget;
class UStatusTextBlock;

UCLASS()
class PROJECTOPENWORLD_API UPalHpWidget_Boss : public UPalHpWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UStatusBarWidget> HpProgressBar{};
	UPROPERTY(meta = (BindWidget))
	TObjectPtr <UTextBlock> MonsterNameTextBlock{};
	UPROPERTY(meta = (BindWidget))
	TObjectPtr < UTextBlock> MonsterLevel{};
	UPROPERTY(meta = (BindWidget))
	TObjectPtr < UStatusTextBlock> MonsterHPText{};
protected:
	virtual void SetupInfo(FName DisplayName, int32 Level) override;
	virtual void UnbindStat() override;
	virtual void BindStat() override;
};
