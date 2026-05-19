#include "GameBase/Component/StatComponent.h"
#include "Pal/Data/PalDamageType.h"
#include "Pal/Data/PalJobTypes.h"


EStatusType PalStatus::GetJobWorkSpeedStatus(EPalJobType JobType)
{
	switch (JobType)
	{
	case EPalJobType::Architecture:
		return EStatusType::Architecture;
		break;
	case EPalJobType::Mining:
		return EStatusType::Mining;
		break;
	case EPalJobType::Lumbering:
		return EStatusType::Lumbering;
		break;
	case EPalJobType::Transport:
		return EStatusType::Transport;
		break;
	case EPalJobType::Attack:
		return EStatusType::Attack;
		break;
	}
	return EStatusType::None;
}


void FStatusValue::SetValue(double NewValue)
{
	double PreValue = Value;
	Value = NewValue;
	if (OnChanged.IsBound())
	{
		OnChanged.Broadcast(PreValue, Value);
	}
}

double FStatusValue::AddValue(double DeltaValue)
{
	double AddValue = DeltaValue;
	if (DeltaValue > 0)
	{
		double Max = std::numeric_limits<double>::max() - DeltaValue;
		if (Max <= Value)
		{
			AddValue = std::numeric_limits<double>::max() - Value;
		}
	}
	else
	{
		if (Value + DeltaValue <= 0)
		{
			AddValue = -Value;
		}
	}
	double PreValue = Value;
	Value += AddValue;
	if (OnChanged.IsBound())
	{
		OnChanged.Broadcast(PreValue, Value);
	}
	return AddValue;
}

void FStatusData::BeginPlay()
{
	CurrentStatValue.SetValue(MaxStatValue.GetValue());
}

void UStatComponent::BeginPlay()
{
	Super::BeginPlay();
	for(auto& Stat : StatusMap)
	{
		Stat.Value.BeginPlay();
	}
}

FOnStatChanged* UStatComponent::GetCurrentOnStatChanged(EStatusType StatName)
{
	if (FStatusData* Stat = StatusMap.Find(StatName))
	{
		return &Stat->CurrentStatValue.OnChanged;
	}
	return nullptr;
}

FOnStatChanged* UStatComponent::GetMaxOnStatChanged(EStatusType StatName)
{
	if (FStatusData* Stat = StatusMap.Find(StatName))
	{
		return &Stat->MaxStatValue.OnChanged;
	}
	return nullptr;
}

void UStatComponent::SetCurrentStat(double Value, EStatusType StatName)
{
	FStatusData* Stat = StatusMap.Find(StatName);
	if (!Stat)
	{
		return;
	}
	Stat->CurrentStatValue.SetValue(Value);
}

void UStatComponent::SetMaxStat(double Value, EStatusType StatName)
{
	FStatusData* Stat = StatusMap.Find(StatName);
	if (!Stat)
	{
		return;
	}
	Stat->MaxStatValue.SetValue(Value);
}

double UStatComponent::AddCurrentStat(double Value, EStatusType StatName)
{
	FStatusData* Stat = StatusMap.Find(StatName);
	if (!Stat)
	{
		return 0.0f;
	}
	return Stat->CurrentStatValue.AddValue(Value);
}

double UStatComponent::AddMaxStat(double Value, EStatusType StatName)
{
	FStatusData* Stat = StatusMap.Find(StatName);
	if (!Stat)
	{
		return 0.0f;
	}
	return Stat->MaxStatValue.AddValue(Value);
}

double UStatComponent::GetCurrentStat(EStatusType StatName) const
{
	if (const FStatusData* Stat = StatusMap.Find(StatName))
	{
		return Stat->CurrentStatValue.GetValue();
	}
	return 0.0;
}
double UStatComponent::GetMaxStat(EStatusType StatName) const
{
	if (const FStatusData* Stat = StatusMap.Find(StatName))
	{
		return Stat->MaxStatValue.GetValue();
	}
	return 0.0;
}

double UStatComponent::GetStatPercent(EStatusType StatName) const
{
	if (const FStatusData* Stat = StatusMap.Find(StatName))
	{
		if (Stat->MaxStatValue.GetValue() <= 0.0 || Stat->CurrentStatValue.GetValue() < 0)
		{
			return 0.0;
		}
		return Stat->CurrentStatValue.GetValue() / Stat->MaxStatValue.GetValue();
	}
	return 0.0;

}

void UStatComponent::ReceiveDamage(const FPalDamagePayload& DamagePayload)
{
	AddCurrentStat(-DamagePayload.BaseDamage, EStatusType::HP);
}
