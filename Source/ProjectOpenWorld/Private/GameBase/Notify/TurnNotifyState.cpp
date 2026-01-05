#include "GameBase/Notify/TurnNotifyState.h"
#include "Creature/Character/BaseMonster.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Pal/Component/PalAttackComponent.h"

#if WITH_EDITOR
void UTurnNotifyState::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& ContainingAnimNotifyEvent)
{
	Super::OnAnimNotifyCreatedInEditor(ContainingAnimNotifyEvent);
	//ContainingAnimNotifyEvent.SetTime(0);
	//if (ContainingAnimNotifyEvent.EndLink.GetLinkedSequence())
	//{
	//	ContainingAnimNotifyEvent.SetDuration(ContainingAnimNotifyEvent.EndLink.GetLinkedSequence()->GetPlayLength());
	//	//UE_LOG(LogTemp, Error, TEXT("EditorCreate %f"), ContainingAnimNotifyEvent.EndLink.GetLinkedSequence()->GetPlayLength());
	//}
}
#endif

void UTurnNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
}

void UTurnNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	ABaseMonster* OwnerCharacter = Cast< ABaseMonster>(MeshComp->GetOwner());
	if (OwnerCharacter)
	{
		if (OwnerCharacter->GetAttackComponent()->GetTargetActor())
		{
			float Dot = FVector::DotProduct(OwnerCharacter->GetActorForwardVector().GetSafeNormal2D(),
				(OwnerCharacter->GetAttackComponent()->GetTargetActor()->GetActorLocation() - OwnerCharacter->GetActorLocation()).GetSafeNormal2D());
			float Angle = FMath::Acos(Dot);
			float Dir = FVector::DotProduct(OwnerCharacter->GetActorRightVector().GetSafeNormal2D(),
				(OwnerCharacter->GetAttackComponent()->GetTargetActor()->GetActorLocation() - OwnerCharacter->GetActorLocation()).GetSafeNormal2D());

			if (Dir < 0)
			{
				Angle = -Angle;
			}
			FRotator NewRot = OwnerCharacter->GetActorRotation();
			NewRot.Yaw += FMath::RadiansToDegrees(Angle) * TurnRate * FrameDeltaTime;
			OwnerCharacter->SetActorRotation(NewRot);
		}
	}
}

void UTurnNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}
