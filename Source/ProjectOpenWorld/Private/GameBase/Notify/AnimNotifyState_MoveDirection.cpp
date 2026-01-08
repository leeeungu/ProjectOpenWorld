#include "GameBase/Notify/AnimNotifyState_MoveDirection.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"

#if WITH_EDITOR
void UAnimNotifyState_MoveDirection::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& ContainingAnimNotifyEvent)
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

void UAnimNotifyState_MoveDirection::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	if (MeshComp && MoveSpeed != 0.f && MeshComp->GetOwner())
	{
		FVector Direction = MoveDirection.GetSafeNormal();
		if (bIsIgnoreZ)
		{
			Direction = MoveDirection.GetSafeNormal2D();
		}
		if (!bIsWorldSpace)
		{
			Direction = MeshComp->GetOwner()->GetActorRotation().RotateVector(Direction);
		}
		FVector DeltaLocation = Direction * MoveSpeed * FrameDeltaTime;
		MeshComp->GetOwner()->AddActorWorldOffset(DeltaLocation, false);
	}
}
