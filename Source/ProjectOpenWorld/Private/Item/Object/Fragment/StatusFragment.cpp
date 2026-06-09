#include "Item/Object/Fragment/StatusFragment.h"
#include "Pal/Character/PalBaseCharacter.h"
#include "GameBase/Component/StatComponent.h"

void UStatusFragment::OnUse(const FItemUseContext& Ctx, FItemUseResult& Out) const
{
	Out.bSuccess = false;
	Out.ConsumeCount = 1;
	if (const APalBaseCharacter* Character = Cast< APalBaseCharacter>(Ctx.User))
	{
		if (UStatComponent* Stat = Character->GetStatComponent())
		{
			for (const auto& Pair : Modifiers)
			{
				Stat->AddMaxStat(Pair.Value, Pair.Key);
				Stat->AddCurrentStat(Pair.Value, Pair.Key);
			}
			Out.bSuccess = false;
		}
	}
}

void UStatusFragment::OnUnUse(const FItemUseContext& Ctx, FItemUseResult& Out) const
{
	Out.bSuccess = false;
	Out.ConsumeCount = 1;
	if (const APalBaseCharacter* Character = Cast< APalBaseCharacter>(Ctx.User))
	{
		if (UStatComponent* Stat = Character->GetStatComponent())
		{
			for (const auto& Pair : Modifiers)
			{
				Stat->AddMaxStat(-Pair.Value, Pair.Key);
				Stat->AddCurrentStat(-Pair.Value, Pair.Key);
			}
			Out.bSuccess = true;
		}
	}
}
