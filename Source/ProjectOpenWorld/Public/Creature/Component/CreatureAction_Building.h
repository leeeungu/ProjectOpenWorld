#pragma once

#include "CoreMinimal.h"
#include "Creature/Component/CreatureActionComponent.h"
#include "CreatureAction_Building.generated.h"

class ABuildingActor;
struct FAIRequestID;
namespace EPathFollowingResult { enum Type : int; }

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTOPENWORLD_API UCreatureAction_Building : public UCreatureActionComponent
{
	GENERATED_BODY()
private:
	TSoftObjectPtr<ABuildingActor> TargetBuilding{};
	bool bActionStart{};
	bool bBuildingStart{};
public:
	UCreatureAction_Building();
	virtual void ActionStart_Implementation(ECreatureActionType ActionType, UObject* TargetObject) override ;
	virtual void ActionEnd_Implementation() override;
protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	void FinishMoved(FAIRequestID RequestID, EPathFollowingResult::Type Result);
};
