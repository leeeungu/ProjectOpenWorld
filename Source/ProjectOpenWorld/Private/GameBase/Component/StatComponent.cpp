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
	CurrentStat = Value;
}

void UStatComponent::SetMaxStat(double Value)
{
	MaxStat = Value;
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
	CurrentStat += AddValue;
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
	MaxStat += AddValue;
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

