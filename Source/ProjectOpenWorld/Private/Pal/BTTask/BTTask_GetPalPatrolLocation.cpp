#include "Pal/BTTask/BTTask_GetPalPatrolLocation.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Pal/Character/BasePalMonster.h"
#include "Pal/Component/PalPatrolComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTask_GetPalPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	ABasePalMonster* PalMonster = Cast<ABasePalMonster>(OwnerComp.GetAIOwner()->GetPawn());
	if (!PalMonster) 
	{
		return EBTNodeResult::Type::Failed;
	}
	UPalPatrolComponent* PatrolComp = PalMonster->GetPalPatrolComponent();
	if (!PatrolComp) 
	{
		return EBTNodeResult::Type::Failed;
	}
	FVector PatrolLocation = PatrolComp->GetCurrentPatrolPoint();
	UE_LOG(LogTemp, Warning, TEXT("Patrol Location: %s"), *PatrolLocation.ToString());
	OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), PatrolLocation);
	PatrolComp->UpdatePatrolIndex();
	return EBTNodeResult::Type::Succeeded;
}
