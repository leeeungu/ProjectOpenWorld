#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Creature/Interface/CreatureActionInterface.h"
#include "CreatureActionComponent.generated.h"

class AAIController;

UCLASS(Abstract, Blueprintable)
class PROJECTOPENWORLD_API UCreatureActionComponent : public UActorComponent, public ICreatureActionInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CreatureAction")
	ECreatureActionType Action{};

	TSoftObjectPtr < AAIController> OwnerController{};
public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "CreatureAction")
	FFinishAction FinishDelegate{};
public:	
	UCreatureActionComponent();

protected:
	virtual void BeginPlay() override;
public:	
	ECreatureActionType GetCreatureAction() { return Action; }
	virtual void ActionStart_Implementation(ECreatureActionType ActionType, UObject* TargetObject) override {}
	virtual void ActionEnd_Implementation() override {}
};
