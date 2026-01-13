#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_GetPalPatrolLocation.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UBTTask_GetPalPatrolLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
