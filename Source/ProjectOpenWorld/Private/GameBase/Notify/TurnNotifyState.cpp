#include "GameBase/Notify/TurnNotifyState.h"
#include "GameBase/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

#if WITH_EDITOR
void UTurnNotifyState::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& ContainingAnimNotifyEvent)
{
	ContainingAnimNotifyEvent.SetTime(0);
	if (ContainingAnimNotifyEvent.EndLink.GetLinkedSequence())
	{
		ContainingAnimNotifyEvent.SetDuration(ContainingAnimNotifyEvent.EndLink.GetLinkedSequence()->GetPlayLength());
		//UE_LOG(LogTemp, Error, TEXT("EditorCreate %f"), ContainingAnimNotifyEvent.EndLink.GetLinkedSequence()->GetPlayLength());
	}
}
#endif

void UTurnNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	ACharacter* OwnerCharacter = Cast< ACharacter>(MeshComp->GetOwner());
	if (OwnerCharacter)
	{
		OwnerCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = false;
	}
}

void UTurnNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UTurnNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	ACharacter* OwnerCharacter = Cast< ACharacter>(MeshComp->GetOwner());
	if (OwnerCharacter)
	{
		OwnerCharacter->GetCharacterMovement()->bUseControllerDesiredRotation= true;
	}
}
