#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Pal/Data/PalAIMonsterState.h"
#include "PalAIStateComponent.generated.h"

class UBlackboardComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAIStateChanged, EPalAIMonsterState, OldState, EPalAIMonsterState, NewState);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UPalAIStateComponent : public UActorComponent
{
	GENERATED_BODY()
	TWeakObjectPtr< UBlackboardComponent> BlackboardComponent{};
protected:
	UPROPERTY(VisibleAnywhere, Category = "PalAIState")
	EPalAIMonsterState CurrentState{};
protected:
	virtual void BeginPlay() override;
	void SetBBValue(EPalAIMonsterState NewState);
public:	
    UPROPERTY(BlueprintAssignable, Category = "PalAIState")
    FOnAIStateChanged OnStateChanged{};

    UFUNCTION(BlueprintCallable, Category = "PalAIState")
    void SetState(EPalAIMonsterState NewState);

    UFUNCTION(BlueprintPure, Category = "PalAIState")
    EPalAIMonsterState GetState() const;

};