#include "GameBase/Component/StatComponent_Level.h"


void UStatComponent_Level::BeginPlay()
{
	Super::BeginPlay();
	if (LevelStatDataTable.IsValid())
	{
		LevelStatDataArray.Empty();
		UDataTable* DT = LevelStatDataTable.LoadSynchronous();
		DT->GetAllRows(TEXT(""), LevelStatDataArray);
		SetMaxStat(LevelStatDataArray[GetCurrentLevel() - 1]->MaxExp);
	}
}

void UStatComponent_Level::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	FName PropertyName = PropertyChangedEvent.GetPropertyName();
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UStatComponent_Level, LevelStatDataTable)||
		PropertyName == GET_MEMBER_NAME_CHECKED(UStatComponent_Level, CurrentLevel))
	{
		if (LevelStatDataTable.IsValid())
		{
			UDataTable* DT = LevelStatDataTable.LoadSynchronous();
			LevelStatDataArray.Empty();
			DT->GetAllRows(TEXT(""), LevelStatDataArray);
			SetCurrentStat(0);
			if (IsMaxLevel())
			{
				CurrentLevel = LevelStatDataArray.Num() - 1;
			}
			if (LevelStatDataArray.IsValidIndex(GetCurrentLevel()))
			{
				SetMaxStat(LevelStatDataArray[GetCurrentLevel() - 1]->MaxExp);
				//AddCurrentStat(LevelStatDataArray[GetCurrentLevel() - 1]->MaxExp);
			}
		}
	}
}

double UStatComponent_Level::AddCurrentStat(double Value)
{
	double Result = Super::AddCurrentStat(Value);
	if (GetMaxStat() <= GetCurrentStat())
	{
		if (LevelStatDataArray.IsValidIndex(CurrentLevel))
		{
			const double MaxExp = LevelStatDataArray[CurrentLevel]->MaxExp;
			int Level = CurrentLevel++;
			if (OnLevelUp.IsBound())
			{
				OnLevelUp.Broadcast(Level, IsMaxLevel());
			}
			SetMaxStat(MaxExp);
			SetCurrentStat(0);
			if(Value - Result > 0)
				Result += AddCurrentStat(Value - Result);
		}
		else if (!LevelStatDataArray.IsEmpty())
		{
			const double MaxExp = LevelStatDataArray.Last()->MaxExp;
			SetMaxStat(MaxExp);
			SetCurrentStat(MaxExp);
		}
	}
	return Result;
}
