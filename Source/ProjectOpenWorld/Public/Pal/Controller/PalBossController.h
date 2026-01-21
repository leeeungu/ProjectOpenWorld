#pragma once

#include "CoreMinimal.h"
#include "Pal/Controller/PalMonsterController.h"
#include "PalBossController.generated.h"

UCLASS()
class PROJECTOPENWORLD_API APalBossController : public APalMonsterController
{
	GENERATED_BODY()
public:
	APalBossController();


public:
	virtual FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId(2); }
};
