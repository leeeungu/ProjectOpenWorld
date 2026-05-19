#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BTTask_MoveToAttackDistance.generated.h"

// AttackInterface를 공격 사거리를 받을 텐데 AcceptableRadius가 bp 상 혼돈을 주기에 없애고 싶어서 HideCategories 함
UCLASS(HideCategories = (Node))
class PROJECTOPENWORLD_API UBTTask_MoveToAttackDistance : public UBTTask_MoveTo
{
	GENERATED_BODY()
private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
