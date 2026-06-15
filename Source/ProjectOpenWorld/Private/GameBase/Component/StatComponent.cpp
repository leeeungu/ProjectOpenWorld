#include "GameBase/Component/StatComponent.h"
#include "Pal/Data/PalDamageType.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Pal/DataTable/PalMonsterData.h"

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
#if WITH_EDITOR
void UStatComponent::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	const FName MemberName = PropertyChangedEvent.MemberProperty
		? PropertyChangedEvent.MemberProperty->GetFName()
		: NAME_None;

	if (MemberName == GET_MEMBER_NAME_CHECKED(UStatComponent, StatusMap))
	{
		// 1) 키가 추가되었는지 판별
		if (PropertyChangedEvent.ChangeType == EPropertyChangeType::ArrayAdd)
		{
			// 2) 추가된 엔트리의 인덱스 획득
			const int32 NewIndex = PropertyChangedEvent.GetArrayIndex(MemberName.ToString());

			// 3) FScriptMapHelper로 해당 키/값 포인터 추출
			if (FMapProperty* MapProp = CastField<FMapProperty>(PropertyChangedEvent.MemberProperty))
			{
				FScriptMapHelper MapHelper(MapProp, MapProp->ContainerPtrToValuePtr<void>(this));
				if (MapHelper.IsValidIndex(NewIndex))
				{
					const EStatusType* AddedKey =
						reinterpret_cast<const EStatusType*>(MapHelper.GetKeyPtr(NewIndex));
					FStatusData* AddedValue =
						reinterpret_cast<FStatusData*>(MapHelper.GetValuePtr(NewIndex));
					if (ACharacter* OwnerCharacter = Cast< ACharacter>(GetOwner()))
					{
						if (AddedKey && *AddedKey == EStatusType::MoveSpeed && OwnerCharacter->GetCharacterMovement())
						{
							AddedValue->MaxStatValue.SetValue(OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed);
						}
					}
					// 이 시점의 AddedKey가 '추가된 키'
				}
			}
		}
	}
}
#endif

FOnStatChanged* UStatComponent::GetCurrentOnStatChanged(EStatusType StatName)
{
	if (FStatusData* Stat = &StatusMap.FindOrAdd(StatName))
	{
		return &Stat->CurrentStatValue.OnChanged;
	}
	return nullptr;
}

FOnStatChanged* UStatComponent::GetMaxOnStatChanged(EStatusType StatName)
{
	if (FStatusData* Stat = &StatusMap.FindOrAdd(StatName))
	{
		return &Stat->MaxStatValue.OnChanged;
	}
	return nullptr;
}

void UStatComponent::StatBeginPlay(EStatusType StatName)
{
	FStatusData* Stat = StatusMap.Find(StatName);
	if (!Stat)
	{
		return;
	}
	Stat->BeginPlay();
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

void UStatComponent::OnReceiveDamage(const FPalDamagePayload& DamagePayload)
{
	AddCurrentStat(-DamagePayload.BaseDamage, EStatusType::HP);
}

void UStatComponent::SetPalLevelData(const FPalMonsterLevelData& LevelData)
{
	for (const auto& Data : LevelData.LevelStatusData)
	{
		EStatusType Stat = Data.Key;
		SetMaxStat(Data.Value, Stat);
		StatBeginPlay(Stat);
	}
}
