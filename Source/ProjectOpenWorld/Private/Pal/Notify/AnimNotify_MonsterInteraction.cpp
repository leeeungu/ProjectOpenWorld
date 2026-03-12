#include "Pal/Notify/AnimNotify_MonsterInteraction.h"
#include "Pal/Character/BossMonster.h"
#include "Pal/Component/PalMonsterInteractionComponent.h"

void UAnimNotify_MonsterInteraction::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (MeshComp)
	{
		ABossMonster* Monster = Cast<ABossMonster>(MeshComp->GetOwner());
		if (Monster && Monster->GetMonsterInteractionComponent())
		{
			Monster->GetMonsterInteractionComponent()->InvokeInteractionEvent(InteractionID, Monster, InteractionEventType);
		}
	}
}
