#pragma once

#include "CoreMinimal.h"
#include "Creature/Component/CreatureActionComponent.h"
#include "CreatureAction_Resource.generated.h"

class AResourceActor;
struct FAIRequestID;
namespace EPathFollowingResult { enum Type : int; }

UCLASS(ClassGroup = (CreatureAction), meta = (BlueprintSpawnableComponent))
class PROJECTOPENWORLD_API UCreatureAction_Resource : public UCreatureActionComponent
{
	GENERATED_BODY()
private:
	TSoftObjectPtr<AResourceActor> TargetActor{};
public:
	UCreatureAction_Resource();

	virtual bool ActionStart_Implementation(ECreatureActionType ActionType, UObject* TargetObject) override;
	virtual bool ActionEnd_Implementation() override;
		

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ExtractResource();
	UFUNCTION()
	void ResetAction();
};
