#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h" 
#include "PalAIController.generated.h"

class ABaseCharacter;
class UPalAIMoveComponent;
enum class EPalAIMonsterState : uint8;
UCLASS()
class PROJECTOPENWORLD_API APalAIController : public AAIController
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PalAIMove")
	TObjectPtr<UPalAIMoveComponent> AIMoveComponent{};

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TObjectPtr<UBehaviorTree> BTree{};
	UPROPERTY()
	TObjectPtr < ABaseCharacter> OwnerPal{};
protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

public:
	APalAIController();

	UFUNCTION(BlueprintPure, Category = "PalAIController")
	FORCEINLINE UPalAIMoveComponent* GetPalAIMoveComponent() const { return AIMoveComponent; }
	
	


};
