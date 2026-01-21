#pragma once

#include "CoreMinimal.h"
#include "Pal/Controller/PalAIController.h"
#include "PalAIlyController.generated.h"

UCLASS()
class PROJECTOPENWORLD_API APalAIlyController : public APalAIController
{
	GENERATED_BODY()

public:
	APalAIlyController();

	virtual FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId(1); }
};
