#include "GameBase/Notify/AnimNotifyState_RotateCharacter.h"

#if WITH_EDITOR
void UAnimNotifyState_RotateCharacter::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& ContainingAnimNotifyEvent)
{
	Super::OnAnimNotifyCreatedInEditor(ContainingAnimNotifyEvent);
	ContainingAnimNotifyEvent.SetTime(0);
	if (ContainingAnimNotifyEvent.EndLink.GetLinkedSequence())
	{
		ContainingAnimNotifyEvent.SetDuration(ContainingAnimNotifyEvent.EndLink.GetLinkedSequence()->GetPlayLength());
		//UE_LOG(LogTemp, Error, TEXT("EditorCreate %f"), ContainingAnimNotifyEvent.EndLink.GetLinkedSequence()->GetPlayLength());
	}
	if (ContainingAnimNotifyEvent.EndLink.GetLinkedMontage())
	{
		ContainingAnimNotifyEvent.SetDuration(ContainingAnimNotifyEvent.EndLink.GetLinkedMontage()->GetPlayLength());
	}
}
#endif

void UAnimNotifyState_RotateCharacter::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		FRotator CurrentRotation{};
		CurrentRotation.Yaw += SpeedYaw * FrameDeltaTime;
		MeshComp->GetOwner()->AddActorWorldRotation(CurrentRotation);
	}
}
