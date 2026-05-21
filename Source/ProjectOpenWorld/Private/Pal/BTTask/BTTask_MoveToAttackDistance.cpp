#include "Pal/BTTask/BTTask_MoveToAttackDistance.h"
#include "GameBase/Interface/AttackInterface.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"

EBTNodeResult::Type UBTTask_MoveToAttackDistance::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = Cast<AAIController>(OwnerComp.GetOwner());
	if (Controller)
	{
		IAttackInterface* Attacker = Cast< IAttackInterface>(Controller->GetPawn());
		if(Attacker)
		{
			AcceptableRadius = Attacker->GetAttackDistance();
		}

	}
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
