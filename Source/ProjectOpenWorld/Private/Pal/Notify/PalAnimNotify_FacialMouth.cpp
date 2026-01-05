
#include "Pal/Notify/PalAnimNotify_FacialMouth.h"

void UPalAnimNotify_FacialMouth::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	// Implement facial mouth notify logic here
	Super::Notify(MeshComp, Animation, EventReference);
	if (MeshComp)
		MeshComp->SetVectorParameterValueOnMaterials(TEXT("MouseOffset"), FVector(0,0,0));
}
