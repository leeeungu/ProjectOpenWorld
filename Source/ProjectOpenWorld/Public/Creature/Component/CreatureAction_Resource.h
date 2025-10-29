#pragma once

#include "CoreMinimal.h"
#include "Creature/Component/CreatureActionComponent.h"
#include "CreatureAction_Resource.generated.h"

class AActor;
struct FAIRequestID;
namespace EPathFollowingResult { enum Type : int; }

UCLASS()
class PROJECTOPENWORLD_API UCreatureAction_Resource : public UCreatureActionComponent
{
	GENERATED_BODY()
private:
	TSoftObjectPtr<AActor> TargetPawn{};
public:
	UCreatureAction_Resource();

	virtual void ActionStart_Implementation(ECreatureActionType ActionType, UObject* TargetObject) override;
	virtual void ActionEnd_Implementation() override;


protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	void FinishMoved(FAIRequestID RequestID, EPathFollowingResult::Type Result);
};
