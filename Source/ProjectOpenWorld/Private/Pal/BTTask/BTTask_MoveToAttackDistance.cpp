#include "Pal/BTTask/BTTask_MoveToAttackDistance.h"
#include "GameBase/Interface/AttackInterface.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"

EBTNodeResult::Type UBTTask_MoveToAttackDistance::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AController* Controller = Cast< AController>(OwnerComp.GetOwner());
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
