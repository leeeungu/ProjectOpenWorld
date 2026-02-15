#include "Pal/Widget/PalHpWidget_MonsterDefault.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Creature/Character/BaseMonster.h"

void UPalHpWidget_MonsterDefault::OnDamageEvent(AActor* Other, float Damage)
{
	if (OwnerMonster->GetMaxHp() > 0)
	{
		HpProgressBar->SetPercent(OwnerMonster->GetCurrentHp() / OwnerMonster->GetMaxHp());
	}
	else
	{
		HpProgressBar->SetPercent(0);
	}
}

void UPalHpWidget_MonsterDefault::InitializeHPWidget(ABaseCharacter* OwnerCharacter)
{
	if (OwnerMonster = Cast<ABaseMonster>(OwnerCharacter))
	{
		OwnerMonster->OnDamagedDelegate.AddUniqueDynamic(this, &UPalHpWidget_MonsterDefault::OnDamageEvent);
		if (MonsterNameTextBlock)
		{
			FText MonsterName = FText::FromString(OwnerMonster->GetMonsterName().ToString());
			//Script/Engine.StringTable'/Game/Pal/StringTable/ST_PalName.ST_PalName'
			MonsterName = FText::FromStringTable("/Game/Monster/StringTable/ST_PalName.ST_PalName", *MonsterName.ToString());
			MonsterNameTextBlock->SetText(MonsterName);
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
	}
}
