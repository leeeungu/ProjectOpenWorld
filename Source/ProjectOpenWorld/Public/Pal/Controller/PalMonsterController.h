#pragma once

#include "CoreMinimal.h"
#include "Pal/Controller/PalAIController.h"
#include "PalMonsterController.generated.h"

class UBehaviorTree;

UCLASS()
class PROJECTOPENWORLD_API APalMonsterController : public APalAIController
{
	GENERATED_BODY()
public:
	APalMonsterController();
public:
	virtual FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId(2); }
};
