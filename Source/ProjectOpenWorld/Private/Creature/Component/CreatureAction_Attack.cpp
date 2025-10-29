#include "Creature/Component/CreatureAction_Attack.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/Character.h"
#include "Creature/Component/CreatureAttackComponent.h"
#include "Engine/DamageEvents.h"
#include "Creature/Interface/CreatureAttackInterface.h"

UCreatureAction_Attack::UCreatureAction_Attack()
{
	Action = ECreatureActionType::Action_Attack;
}

void UCreatureAction_Attack::ActionStart_Implementation(ECreatureActionType ActionType, UObject* TargetObject)
{	
	TargetPawn = Cast< APawn>(TargetObject);
	if (!TargetPawn || !OwnerController || bAttackable)
		return;
	bAttackable = true;
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
	OwnerController->StopMovement();
	bAttackable = false;
}

void UCreatureAction_Attack::BeginPlay()
{
	UCreatureActionComponent::BeginPlay();
	if (OwnerController)
		OwnerController->ReceiveMoveCompleted.AddDynamic(this, &UCreatureAction_Attack::FinishMoved);
}

void UCreatureAction_Attack::FinishMoved(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (EPathFollowingResult::Type::Success == Result && TargetPawn && bAttackable)
	{
		TScriptInterface<ICreatureAttackInterface> Character = TScriptInterface<ICreatureAttackInterface>(GetOwner());
		if (Character)
		{
			UCreatureAttackComponent * Component = ICreatureAttackInterface::Execute_GetAttackComponent(GetOwner());
			if (Component)
			{
				FCreatureAttackData Data{};
				if (Component->GetAttackData(0, Data))
				{
					Component->SetAttackIndex(0);
					Component->PlayAttackMontage();
					FTimerHandle Handle{ };
					GetWorld()->GetTimerManager().SetTimer(Handle, this, &UCreatureAction_Attack::PlayAttack, Data.AttackDelay);
					
				}
			}
		}
	}

	if (EPathFollowingResult::Type::Success == Result && !TargetPawn)
	{
		bAttackable = false;
	}
}


void UCreatureAction_Attack::PlayAttack()
{
	if (!TargetPawn)
	{
		bAttackable = false;
		return;
	}
	if(!bAttackable)
		return;


	FAIMoveRequest MoveReq(TargetPawn.Get());
	MoveReq.SetUsePathfinding(true);
	MoveReq.SetAllowPartialPath(true);
	MoveReq.SetAcceptanceRadius(100.0f);
	MoveReq.SetReachTestIncludesAgentRadius(true);
	MoveReq.SetCanStrafe(true);
	OwnerController->MoveTo(MoveReq);
}
