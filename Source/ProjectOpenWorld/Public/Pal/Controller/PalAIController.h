#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h" 
#include "PalAIController.generated.h"

class ABaseCharacter;
class UPalAIMoveComponent;

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

	UFUNCTION()
	void RecieveDamage(const struct FPalDamagePayload& DamagePayload);
public:
	APalAIController();

	UFUNCTION(BlueprintPure, Category = "Pal AI Controller")
	FORCEINLINE UPalAIMoveComponent* GetPalAIMoveComponent() const { return AIMoveComponent; }
};
