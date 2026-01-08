#include "Pal/Notify/PalAnimNotify_FacialMouth.h"
#include "Pal/FunctionLibrary/Function_PalFacial.h"

void UPalAnimNotify_FacialMouth::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	// Implement facial mouth notify logic here
	Super::Notify(MeshComp, Animation, EventReference);
	UFunction_PalFacial::SetPalFacialMouthOffset(MeshComp, MouthType);
}
