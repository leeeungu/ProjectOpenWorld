#include "GameBase/Notify/AnimNotifyState_SetControllerRot.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#if WITH_EDITOR
void UAnimNotifyState_SetControllerRot::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& ContainingAnimNotifyEvent)
{
	Super::OnAnimNotifyCreatedInEditor(ContainingAnimNotifyEvent);
	ContainingAnimNotifyEvent.SetTime(0);
	if (ContainingAnimNotifyEvent.EndLink.GetLinkedSequence())
	{
		ContainingAnimNotifyEvent.SetDuration(ContainingAnimNotifyEvent.EndLink.GetLinkedSequence()->GetPlayLength());
		//UE_LOG(LogTemp, Error, TEXT("EditorCreate %f"), ContainingAnimNotifyEvent.EndLink.GetLinkedSequence()->GetPlayLength());
	}
}
#endif

void UAnimNotifyState_SetControllerRot::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	ACharacter* OwnerCharacter = Cast< ACharacter>(MeshComp->GetOwner());
	if (OwnerCharacter)
	{
		bPreviousUseControllerDesiredRotation = OwnerCharacter->GetCharacterMovement()->bUseControllerDesiredRotation;
		OwnerCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = bNewUseControllerDesiredRotation;
	}
}

void UAnimNotifyState_SetControllerRot::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UAnimNotifyState_SetControllerRot::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	ACharacter* OwnerCharacter = Cast< ACharacter>(MeshComp->GetOwner());
	if (OwnerCharacter)
	{
		OwnerCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = bPreviousUseControllerDesiredRotation;
	}
}
