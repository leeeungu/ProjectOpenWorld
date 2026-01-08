#include "Pal/Notify/AnimNotify_PatternEvent.h"
#include "Pal/Character/BossMonster.h"
#include "Pal/Component/PalPatternComponent.h"

void UAnimNotify_PatternEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (MeshComp)
	{
		ABossMonster* Boss = Cast<ABossMonster>(MeshComp->GetOwner());
		if (Boss)
		{
			if (PatternState == EPalPatternEventType::PatternEnd)
				Boss->GetPatternComponent()->EndPattern(Index);
			else if (PatternState == EPalPatternEventType::PatternStart)
				Boss->GetPatternComponent()->StartPattern(Index);
		}
	}
}
