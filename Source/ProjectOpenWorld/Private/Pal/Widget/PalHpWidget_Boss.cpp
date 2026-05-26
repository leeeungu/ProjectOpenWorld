#include "Pal/Widget/PalHpWidget_Boss.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

//void UPalHpWidget_Boss::OnDamageEvent(AActor* Other, float Damage)
//{
//	
//}

//void UPalHpWidget_Boss::InitializeHPWidget(ABaseCharacter* OwnerCharacter)
//{
//	if (OwnerMonster = Cast<ABaseMonster>(OwnerCharacter))
//	{
//		//OwnerMonster->OnDamagedDelegate.AddUniqueDynamic(this, &UPalHpWidget_Boss::OnDamageEvent);
//		if (MonsterNameTextBlock)
//		{
//			FText MonsterName = FText::FromString(OwnerMonster->GetMonsterName().ToString());
//			//Script/Engine.StringTable'/Game/Pal/StringTable/ST_PalName.ST_PalName'
//			MonsterName = FText::FromStringTable("/Game/Monster/StringTable/ST_PalName.ST_PalName", *MonsterName.ToString());
//			MonsterNameTextBlock->SetText(MonsterName);
//		}
//		if (MonsterLevel)
//		{
//			FText LevelText = FText::FromString(TEXT("Lv.") + FString::FormatAsNumber(OwnerMonster->GetMonsterLevel()));
//			MonsterLevel->SetText(LevelText);
//		}
//		if (OwnerMonster->GetMaxHp() > 0)
//		{
//			HpProgressBar->SetPercent(OwnerMonster->GetCurrentHp() / OwnerMonster->GetMaxHp());
//		}
//		else
//		{
//			HpProgressBar->SetPercent(0);
//		}
//		FString HpText = FString::FormatAsNumber(OwnerMonster->GetCurrentHp()) + TEXT(" / ")
//			+ FString::FormatAsNumber(OwnerMonster->GetMaxHp());
//		MonsterHPText->SetText(FText::FromString(HpText));
//	}
//}

void UPalHpWidget_Boss::OnHPChanged(double PreStat, double CurStat)
{
	FString HpText = FString::FormatAsNumber(CurStat) + TEXT(" / ") + FString::FormatAsNumber(MaxHP);
	if (!HpProgressBar || !MonsterHPText)
		return;
	MonsterHPText->SetText(FText::FromString(HpText));
	if (MaxHP > 0)
	{
		HpProgressBar->SetPercent(CurStat / MaxHP);
	}
	else
	{
		HpProgressBar->SetPercent(0);
	}
}

void UPalHpWidget_Boss::InitializeHPWidget(FName PalID, int32 Level, double CurStat, double _MaxHP)
{
	if (MonsterNameTextBlock)
	{
		FText MonsterName = FText::FromString(PalID.ToString());
		//Script/Engine.StringTable'/Game/Pal/StringTable/ST_PalName.ST_PalName'
		MonsterName = FText::FromStringTable("/Game/Monster/StringTable/ST_PalName.ST_PalName", *MonsterName.ToString());
		MonsterNameTextBlock->SetText(MonsterName);
	}
	if (MonsterLevel)
	{
		FText LevelText = FText::FromString(TEXT("Lv.") + FString::FormatAsNumber(Level));
		MonsterLevel->SetText(LevelText);
	}
	MaxHP = _MaxHP;
	FString HpText = FString::FormatAsNumber(CurStat) + TEXT(" / ") + FString::FormatAsNumber(MaxHP);
	if(MonsterHPText)
		MonsterHPText->SetText(FText::FromString(HpText));
	if (!HpProgressBar)
		return;
	if (MaxHP > 0)
	{
		HpProgressBar->SetPercent(CurStat / MaxHP);
	}
	else
	{
		HpProgressBar->SetPercent(0);
	}
}
