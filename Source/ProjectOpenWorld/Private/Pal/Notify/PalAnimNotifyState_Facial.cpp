#include "Pal/Notify/PalAnimNotifyState_Facial.h"
#include "Pal/FunctionLibrary/Function_PalFacial.h"

void UPalAnimNotifyState_Facial::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& ContainingAnimNotifyEvent)
{
	Super::OnAnimNotifyCreatedInEditor(ContainingAnimNotifyEvent);
	ContainingAnimNotifyEvent.SetTime(0);
	if (ContainingAnimNotifyEvent.EndLink.GetLinkedSequence())
	{
		ContainingAnimNotifyEvent.SetDuration(ContainingAnimNotifyEvent.EndLink.GetLinkedSequence()->GetPlayLength());
	}
	if (ContainingAnimNotifyEvent.EndLink.GetLinkedMontage())
	{
		ContainingAnimNotifyEvent.SetDuration(ContainingAnimNotifyEvent.EndLink.GetLinkedMontage()->GetPlayLength());
	}
}

void UPalAnimNotifyState_Facial::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	UFunction_PalFacial::SetPalFacialEyeOffset(MeshComp, StartEyeType);
	UFunction_PalFacial::SetPalFacialMouthOffset(MeshComp, StartMouthType);
}

void UPalAnimNotifyState_Facial::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	UFunction_PalFacial::SetPalFacialEyeOffset(MeshComp, EndEyeType);
	UFunction_PalFacial::SetPalFacialMouthOffset(MeshComp, EndMouthType);
}
