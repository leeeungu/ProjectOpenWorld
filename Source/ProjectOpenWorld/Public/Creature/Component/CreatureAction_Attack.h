#pragma once

#include "CoreMinimal.h"
#include "Creature/Component/CreatureActionComponent.h"
#include "CreatureAction_Attack.generated.h"

class APawn;
struct FAIRequestID;
namespace EPathFollowingResult { enum Type : int; }

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTOPENWORLD_API UCreatureAction_Attack : public UCreatureActionComponent
{
	GENERATED_BODY()
private:
	TSoftObjectPtr<APawn> TargetPawn{};
protected:
	int Count{};
	bool bAttackable{};
public:
	UCreatureAction_Attack();

	virtual void ActionStart_Implementation(ECreatureActionType ActionType, UObject* TargetObject) override;
	virtual void ActionEnd_Implementation() override;


protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	void FinishMoved(FAIRequestID RequestID, EPathFollowingResult::Type Result);
	UFUNCTION()
	void PlayAttack();
};
