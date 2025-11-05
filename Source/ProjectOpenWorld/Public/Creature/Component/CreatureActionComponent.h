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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CreatureAction")
	ECreatureActionType Action{};
	bool bActionStart{};
public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "CreatureAction")
	FFinishAction FinishDelegate{};
public:	
	UCreatureActionComponent();

protected:
	virtual void BeginPlay() override;
public:	
	ECreatureActionType GetCreatureAction() { return Action; }
	virtual bool ActionStart_Implementation(AActor* SendActor, AActor* TargetActor = nullptr) override { return false; }
	virtual bool ActionEnd_Implementation() override { return true; }
};
