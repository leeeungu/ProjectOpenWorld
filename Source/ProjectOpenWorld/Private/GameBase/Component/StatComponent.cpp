#include "GameBase/Component/StatComponent.h"

UStatComponent::UStatComponent() : Super()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UStatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UStatComponent::SetCurrentStat(double Value)
{
	double PreCurrentStat = CurrentStat;
	CurrentStat = Value;
	if (OnStatChanged.IsBound())
	{
		OnStatChanged.Broadcast(PreCurrentStat, MaxStat);
	}
}

void UStatComponent::SetMaxStat(double Value)
{
	double PreMaxStat = MaxStat;
	MaxStat = Value;
	if (OnStatChanged.IsBound())
	{
		OnStatChanged.Broadcast(CurrentStat, PreMaxStat);
	}
}

double UStatComponent::AddCurrentStat(double Value)
{
	double AddValue = Value;
	if (Value)
	{
		double Max = MaxStat - Value;
		if (Max <= CurrentStat)
		{
			AddValue = MaxStat - CurrentStat;
		}
	}
	else
	{
		if (CurrentStat + Value <= 0)
		{
			AddValue = -CurrentStat;
		}
	}
	double PreCurrentStat = CurrentStat;
	CurrentStat += AddValue;
	if (OnStatChanged.IsBound())
	{
		OnStatChanged.Broadcast(PreCurrentStat, MaxStat);
	}
	return AddValue;
}

double UStatComponent::AddMaxStat(double Value)
{
	double AddValue = Value;
	if (Value)
	{
		double Max = std::numeric_limits<double>::max() - Value;
		if (Max <= MaxStat)
		{
			AddValue = std::numeric_limits<double>::max() - Value;
		}
	}
	else
	{
		if (MaxStat + Value <= 0)
		{
			AddValue = -MaxStat;
		}
	}
	double PreMaxStat = MaxStat;
	MaxStat += AddValue;
	if (OnStatChanged.IsBound())
	{
		OnStatChanged.Broadcast(CurrentStat, PreMaxStat);
	}
	return AddValue;
}

double UStatComponent::GetStatPercent() const
{
	if (MaxStat <= 0.0 || CurrentStat < 0)
	{
		return 0.0;
	}
	return CurrentStat / MaxStat;
}

