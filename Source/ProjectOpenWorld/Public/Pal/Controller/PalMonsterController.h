#pragma once

#include "CoreMinimal.h"
#include "Pal/Controller/PalAIController.h"
#include "PalMonsterController.generated.h"

class UBehaviorTree;

UCLASS()
class PROJECTOPENWORLD_API APalMonsterController : public APalAIController
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TObjectPtr<UBehaviorTree> MonsterBTree{};
public:
	APalMonsterController();

protected:
	virtual void BeginPlay() override;
};
