#include "Pal/Notify/AnimNotify_PatternEvent_Anubis.h"

void UAnimNotify_PatternEvent_Anubis::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Index = static_cast<uint8>(AnubisPatternEventType);
	Super::Notify(MeshComp, Animation, EventReference);
}
