#include "Pal/Notify/AnimNotify_MonsterInteraction.h"
#include "Creature/Character/BaseMonster.h"
#include "Pal/Component/PalMonsterInteractionComponent.h"

void UAnimNotify_MonsterInteraction::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (MeshComp)
	{
		ABaseMonster* Monster = Cast<ABaseMonster>(MeshComp->GetOwner());
		if (Monster && Monster->GetMonsterInteractionComponent())
		{
			Monster->GetMonsterInteractionComponent()->InvokeInteractionEvent(InteractionID, Monster);
		}
	}
}
