// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pal/Widget/PalHpWidget.h"
#include "PalHpWidget_Boss.generated.h"

class UTextBlock;
class UProgressBar;
class ABaseMonster;

UCLASS()
class PROJECTOPENWORLD_API UPalHpWidget_Boss : public UPalHpWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HpProgressBar{};
	UPROPERTY(meta = (BindWidget))
	TObjectPtr <UTextBlock> MonsterNameTextBlock{};
	UPROPERTY(meta = (BindWidget))
	TObjectPtr < UTextBlock> MonsterLevel{};
	UPROPERTY(meta = (BindWidget))
	TObjectPtr < UTextBlock> MonsterHPText{};
	TObjectPtr < ABaseMonster> OwnerMonster{};
protected:
	virtual void OnDamageEvent(AActor* Other, float Damage) override;
public:
	virtual  void InitializeHPWidget(ABaseCharacter* OwnerCharacter) override;
};
