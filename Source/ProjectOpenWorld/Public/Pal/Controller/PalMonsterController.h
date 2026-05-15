#pragma once

#include "CoreMinimal.h"
#include "Pal/Controller/PalAIController.h"
#include "PalMonsterController.generated.h"

class UPalAIStateComponent;

UCLASS()
class PROJECTOPENWORLD_API APalMonsterController : public APalAIController
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, Category = "PalAIState")
	TObjectPtr<UPalAIStateComponent> AIStateComponent{};

public:
	APalMonsterController();
	virtual FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId(2); }
	UFUNCTION(BlueprintPure, Category = "PalAIState")
	UPalAIStateComponent* GetAIStateComponent() const { return AIStateComponent; }
};
