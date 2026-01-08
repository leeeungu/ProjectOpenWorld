#include "Pal/Notify/PalAnimNotify_FacialEye.h"
#include "Pal/FunctionLibrary/Function_PalFacial.h"

void UPalAnimNotify_FacialEye::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	// Implement facial eye notify logic here
	Super::Notify(MeshComp, Animation, EventReference);
	UFunction_PalFacial::SetPalFacialEyeOffset(MeshComp, EyeType);
}
