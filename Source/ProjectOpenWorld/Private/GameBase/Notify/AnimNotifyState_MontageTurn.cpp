#include "GameBase/Notify/AnimNotifyState_MontageTurn.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#if WITH_EDITOR
void UAnimNotifyState_MontageTurn::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& ContainingAnimNotifyEvent)
{
	Super::OnAnimNotifyCreatedInEditor(ContainingAnimNotifyEvent);
}
#endif

void UAnimNotifyState_MontageTurn::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
}

void UAnimNotifyState_MontageTurn::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	ACharacter* OwnerCharacter = MeshComp->GetOwner() ? Cast< ACharacter>(MeshComp->GetOwner()) : nullptr;
	AController* OwnerController = OwnerCharacter ? OwnerCharacter->GetController() : nullptr;
	if (OwnerCharacter && OwnerController)
	{
		FVector ConrollerForward = OwnerController->GetControlRotation().Quaternion().GetForwardVector().GetSafeNormal2D();
		FVector CharacterForward = OwnerCharacter->GetActorForwardVector().GetSafeNormal2D();
		if (!ConrollerForward.Equals(CharacterForward))
		{
			float Dir = FVector::DotProduct(OwnerCharacter->GetActorRightVector().GetSafeNormal2D(), ConrollerForward);
			float Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(CharacterForward, ConrollerForward)));
			float TurnDelta = TurnRate * FrameDeltaTime;
			if (TurnDelta > abs(Angle))
				TurnDelta = Angle;
			if (Dir < 0)
			{
				TurnDelta = -TurnDelta;
			}
			FRotator NewRot = OwnerCharacter->GetActorRotation();
			NewRot.Yaw += TurnDelta;
			OwnerCharacter->SetActorRotation(NewRot);
		}
	}
}

void UAnimNotifyState_MontageTurn::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}
