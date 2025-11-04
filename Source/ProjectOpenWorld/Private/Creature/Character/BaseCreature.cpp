#include "Creature/Character/BaseCreature.h"
#include "Creature/Component/CreatureAction_Building.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"

void ABaseCreature::BeginPlay()
{
	ABaseCharacter::BeginPlay();
	TSet<UActorComponent*> Coms = GetComponents();
	for (auto Component : Coms)
	{
		UCreatureActionComponent* Action = Cast< UCreatureActionComponent>(Component);
		if (Action)
		{
			ActionComponents[(uint8)Action->GetCreatureAction()] = TScriptInterface<ICreatureActionInterface>(Component);
		}
	}

	AAIController* OwnerController = Cast<AAIController>(GetController());
	if (OwnerController)
	{
		OwnerController->ReceiveMoveCompleted.AddDynamic(this, &ABaseCreature::FinishActionMove);
	}
}

void ABaseCreature::FinishActionMove(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (Result != EPathFollowingResult::Type::Success)
		return;
	ActionType = NextActionType;
	NextActionType = ECreatureActionType::Action_None;
	if (ActionComponents[(uint8)ActionType] && ActionComponents[(uint8)ActionType].GetObject())
	{
		if (ICreatureActionInterface::Execute_ActionStart(ActionComponents[(uint8)ActionType].GetObject(), ActionType, TargetObj.Get()) == false)
		{
			ResetAction();
		}
	}
}

bool ABaseCreature::MoveToTarget()
{
	AAIController* OwnerController = Cast<AAIController>(GetController());
	if (AActor* TargetActor = Cast< AActor>(TargetObj.Get()))
	{
		FAIMoveRequest MoveReq(TargetActor);
		MoveReq.SetUsePathfinding(true);
		MoveReq.SetAllowPartialPath(false);
		MoveReq.SetAcceptanceRadius(100.0f);
		MoveReq.SetReachTestIncludesAgentRadius(true);
		if (OwnerController)
		{
			FNavPathSharedPtr OutPath{};
			OwnerController->MoveTo(MoveReq, &OutPath);
			if (!OutPath.IsValid())
				return false;
		}
	}
	return true;
}

void ABaseCreature::ReceiveMessage_Implementation(EMessageType MessageType, AActor* SendActor, UObject* TargetObject)
{
}

void ABaseCreature::ReceiveActionMessage_Implementation(ECreatureActionType MessageType, AActor* SendActor, UObject* TargetObject)
{
	AAIController* OwnerController = Cast<AAIController>(GetController());
	if (ActionComponents[(uint8)ActionType] &&
		ActionComponents[(uint8)ActionType].GetObject())
	{
		ICreatureActionInterface::Execute_ActionEnd(ActionComponents[(uint8)ActionType].GetObject());
	}
	ResetAction();
	OwnerController->StopMovement();
	if (MessageType != ECreatureActionType::Action_None)
	{
		ActionType = ECreatureActionType::Action_Move;
		NextActionType = MessageType;
		TargetObj = TargetObject;
		if (!MoveToTarget())
		{
			ResetAction();
		}
	}
}

float ABaseCreature::GetAttackDamage_Implementation() const
{
	{
		//return AttackComponent.Get()->GetDamage();
	}
	return 0.0f;
}

void ABaseCreature::ResetAction()
{
	ActionType = ECreatureActionType::Action_None;
	NextActionType = ECreatureActionType::Action_None;
	TargetObj = nullptr;
}
