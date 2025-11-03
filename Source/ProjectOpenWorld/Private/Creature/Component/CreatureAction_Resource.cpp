#include "Creature/Component/CreatureAction_Resource.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/Character.h"


UCreatureAction_Resource::UCreatureAction_Resource()
{
	Action = ECreatureActionType::Action_Mining;
}

void UCreatureAction_Resource::ActionStart_Implementation(ECreatureActionType ActionType, UObject* TargetObject)
{
	if (TargetActor != TargetObject)
	{
		bActionStart = false;
	}
	TargetActor = Cast< APawn>(TargetObject);
	if (!TargetActor || !OwnerController || bActionStart)
		return;
	bActionStart = false;
	FAIMoveRequest MoveReq(TargetActor.Get());
	MoveReq.SetUsePathfinding(true);
	MoveReq.SetAllowPartialPath(true);
	MoveReq.SetAcceptanceRadius(100.0f);
	MoveReq.SetReachTestIncludesAgentRadius(true);
	MoveReq.SetCanStrafe(true);
	OwnerController->MoveTo(MoveReq);
}

void UCreatureAction_Resource::ActionEnd_Implementation()
{
	OwnerController->StopMovement();
}

void UCreatureAction_Resource::BeginPlay()
{
	UCreatureActionComponent::BeginPlay();
	if (OwnerController)
		OwnerController->ReceiveMoveCompleted.AddDynamic(this, &UCreatureAction_Resource::FinishMoved);
}

void UCreatureAction_Resource::FinishMoved(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (EPathFollowingResult::Type::Success == Result && TargetActor)
	{
		//TScriptInterface<ICreatureAttackInterface> Character = TScriptInterface<ICreatureAttackInterface>(GetOwner());
		//if (Character)
		//{
		//	UCreatureAttackComponent* Component = ICreatureAttackInterface::Execute_GetAttackComponent(GetOwner());
		//	if (Component)
		//	{
		//		FCreatureAttackData Data{};
		//		if (Component->GetAttackData(0, Data))
		//		{
		//			Component->SetAttackIndex(0);
		//			Component->PlayAttackMontage();
		//			FTimerHandle Handle{ };
		//			GetWorld()->GetTimerManager().SetTimer(Handle, this, &UCreatureAction_Attack::PlayAttack, Data.AttackDelay);
		//
		//		}
		//	}
		//}
	}
	//if (!TargetPawn)
	//	return;
	//FAIMoveRequest MoveReq(TargetPawn.Get());
	//MoveReq.SetUsePathfinding(true);
	//MoveReq.SetAllowPartialPath(true);
	//MoveReq.SetAcceptanceRadius(100.0f);
	//MoveReq.SetReachTestIncludesAgentRadius(true);
	//MoveReq.SetCanStrafe(true);
	//OwnerController->MoveTo(MoveReq);
}
