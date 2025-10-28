#include "Creature/Component/CreatureAction_Attack.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/Character.h"

UCreatureAction_Attack::UCreatureAction_Attack()
{
	Action = ECreatureActionType::Action_Attack;
}

void UCreatureAction_Attack::ActionStart_Implementation(ECreatureActionType ActionType, UObject* TargetObject)
{	
	if (TargetPawn)
		return;
	TargetPawn = Cast< APawn>(TargetObject);
	if (!TargetPawn || !OwnerController)
		return;
	FAIMoveRequest MoveReq(TargetPawn.Get());
	MoveReq.SetUsePathfinding(true);
	MoveReq.SetAllowPartialPath(true);
	MoveReq.SetAcceptanceRadius(100.0f);
	MoveReq.SetReachTestIncludesAgentRadius(true);
	MoveReq.SetCanStrafe(true);
	OwnerController->MoveTo(MoveReq);
}

void UCreatureAction_Attack::ActionEnd_Implementation()
{
}

void UCreatureAction_Attack::BeginPlay()
{
	UCreatureActionComponent::BeginPlay();
	if (OwnerController)
		OwnerController->ReceiveMoveCompleted.AddDynamic(this, &UCreatureAction_Attack::FinishMoved);
}

void UCreatureAction_Attack::FinishMoved(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (EPathFollowingResult::Type::Success == Result && TargetPawn)
	{
		ACharacter* Character = Cast<ACharacter>(GetOwner());
		if (Character && Montage)
		{
			Character->GetMesh()->GetAnimInstance()->Montage_Play(Montage);
			FTimerHandle Handle{ };
			GetWorld()->GetTimerManager().SetTimer(Handle,this, &UCreatureAction_Attack::PlayAttack,5);
		}
	}
}

void UCreatureAction_Attack::PlayAttack()
{
	if (!TargetPawn)
		return;

	FAIMoveRequest MoveReq(TargetPawn.Get());
	MoveReq.SetUsePathfinding(true);
	MoveReq.SetAllowPartialPath(true);
	MoveReq.SetAcceptanceRadius(100.0f);
	MoveReq.SetReachTestIncludesAgentRadius(true);
	MoveReq.SetCanStrafe(true);
	OwnerController->MoveTo(MoveReq);
}
