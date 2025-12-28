#include "GameBase/Notify/AnimNotifyState_SetMovementMode.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UAnimNotifyState_SetMovementMode::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& ContainingAnimNotifyEvent)
{
	Super::OnAnimNotifyCreatedInEditor(ContainingAnimNotifyEvent);
	ContainingAnimNotifyEvent.SetTime(0);
	if (ContainingAnimNotifyEvent.EndLink.GetLinkedSequence())
	{
		ContainingAnimNotifyEvent.SetDuration(ContainingAnimNotifyEvent.EndLink.GetLinkedSequence()->GetPlayLength());
		//UE_LOG(LogTemp, Error, TEXT("EditorCreate %f"), ContainingAnimNotifyEvent.EndLink.GetLinkedSequence()->GetPlayLength());
	}
}

void UAnimNotifyState_SetMovementMode::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	ACharacter* OwnerCharacter = Cast< ACharacter>(MeshComp->GetOwner());
	if (OwnerCharacter)
	{
		OwnerCharacter->GetCharacterMovement()->SetMovementMode(NewMovementMode, NewCustomMode);
	}
}

void UAnimNotifyState_SetMovementMode::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation,  EventReference);
	ACharacter* OwnerCharacter = Cast< ACharacter>(MeshComp->GetOwner());
	if (OwnerCharacter)
	{
		OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking, 0);
	}

}
