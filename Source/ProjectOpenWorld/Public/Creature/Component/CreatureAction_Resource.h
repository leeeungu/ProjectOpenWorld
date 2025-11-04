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
	TSoftObjectPtr<AActor> TargetActor{};
public:
	UCreatureAction_Resource();

	virtual bool ActionStart_Implementation(ECreatureActionType ActionType, UObject* TargetObject) override;
	virtual bool ActionEnd_Implementation() override;
		

protected:
	virtual void BeginPlay() override;
};
