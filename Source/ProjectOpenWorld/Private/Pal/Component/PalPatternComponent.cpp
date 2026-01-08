#include "Pal/Component/PalPatternComponent.h"
#include "Pal/Object/PatternObj.h"
#include "Pal/Component/PalAttackComponent.h"
#include "Creature/Character/BaseMonster.h"

UPalPatternComponent::UPalPatternComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPalPatternComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	TArray<TObjectPtr<UPatternObj>> Array = PatternSet.Array();

	for (TObjectPtr<UPatternObj> PatternObj : Array)
	{
		if (PatternObj)
		{
			PatternObj->UpdatePattern(DeltaTime);
		}
	}
}

void UPalPatternComponent::StartPattern(uint8 PatternIndex)
{
	if (PatternClassArray.IsValidIndex(PatternIndex))
	{
		TObjectPtr<UPatternObj> PatternObj = PatternClassArray[PatternIndex];
		if (PatternObj && !PatternSet.Contains(PatternObj) && !PatternObj->IsPatternActive())
		{
			ABaseCharacter* OwnerCharacter = Cast<ABaseCharacter>(GetOwner());
			TObjectPtr<UPalAttackComponent> AttackComp = OwnerCharacter ? OwnerCharacter->FindComponentByClass<UPalAttackComponent>() : nullptr;
			AActor* TargetActor = AttackComp ? AttackComp->GetTargetActor() : nullptr;
			PatternObj->InitializePattern(GetOwner(), TargetActor);
			PatternObj->StartPattern();
			PatternSet.Add(PatternObj);
		}
	}
}

void UPalPatternComponent::EndPattern(uint8 PatternIndex)
{
	if (PatternClassArray.IsValidIndex(PatternIndex))
	{
		TObjectPtr<UPatternObj> PatternObj = PatternClassArray[PatternIndex];
		if (PatternObj && PatternSet.Contains(PatternObj))
		{
			PatternObj->EndPattern();
			PatternSet.Remove(PatternObj);
		}
	}
}

void UPalPatternComponent::UpdatePatternCondition(uint8 PatternIndex)
{
	if (PatternClassArray.IsValidIndex(PatternIndex))
	{
		TObjectPtr<UPatternObj> PatternObj = PatternClassArray[PatternIndex];
		if (PatternObj && PatternSet.Contains(PatternObj) && PatternObj->IsPatternActive())
		{
			PatternObj->UpdateCondition();
		}
	}
}

bool UPalPatternComponent::IsPatternActive(uint8 PatternIndex) const
{
	if (PatternClassArray.IsValidIndex(PatternIndex))
	{
		TObjectPtr<UPatternObj> PatternObj = PatternClassArray[PatternIndex];
		if (PatternObj && PatternSet.Contains(PatternObj))
		{
			return PatternObj->IsPatternActive();
		}
	}
	return false;
}
