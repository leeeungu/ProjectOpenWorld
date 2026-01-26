#include "Pal/Widget/PalHpWidget_Boss.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Creature/Character/BaseMonster.h"

void UPalHpWidget_Boss::OnDamageEvent(AActor* Other, float Damage)
{
	FString HpText = FString::FormatAsNumber(OwnerMonster->GetCurrentHp()) + TEXT(" / ")
		+ FString::FormatAsNumber(OwnerMonster->GetMaxHp());
	MonsterHPText->SetText(FText::FromString(HpText));
	if (OwnerMonster->GetMaxHp() > 0)
	{
		HpProgressBar->SetPercent(OwnerMonster->GetCurrentHp() / OwnerMonster->GetMaxHp());
	}
	else
	{
		HpProgressBar->SetPercent(0);
	}
}

void UPalHpWidget_Boss::InitializeHPWidget(ABaseCharacter* OwnerCharacter)
{
	if (OwnerMonster = Cast<ABaseMonster>(OwnerCharacter))
	{
		OwnerMonster->OnDamagedDelegate.AddUniqueDynamic(this, &UPalHpWidget_Boss::OnDamageEvent);
		if (MonsterNameTextBlock)
		{
			MonsterNameTextBlock->SetText(FText::FromName(OwnerMonster->GetMonsterName()));
		}
		if (MonsterLevel)
		{
			FText LevelText = FText::FromString(TEXT("Lv.") + FString::FormatAsNumber(OwnerMonster->GetMonsterLevel()));
			MonsterLevel->SetText(LevelText);
		}
		if (OwnerMonster->GetMaxHp() > 0)
		{
			HpProgressBar->SetPercent(OwnerMonster->GetCurrentHp() / OwnerMonster->GetMaxHp());
		}
		else
		{
			HpProgressBar->SetPercent(0);
		}
		FString HpText = FString::FormatAsNumber(OwnerMonster->GetCurrentHp()) + TEXT(" / ")
			+ FString::FormatAsNumber(OwnerMonster->GetMaxHp());
		MonsterHPText->SetText(FText::FromString(HpText));
	}
}