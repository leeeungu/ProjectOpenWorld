#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h" 
#include "PalAIController.generated.h"

class ABaseCharacter;

UCLASS()
class PROJECTOPENWORLD_API APalAIController : public AAIController
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TObjectPtr<UBehaviorTree> BTree{};

	bool bIsMove{};

	UPROPERTY()
	TObjectPtr < ABaseCharacter> OwnerPal{};
protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
private:
	EPathFollowingRequestResult::Type Resut{};

protected:
	bool FindLandscapeBelow(FVector StartLocation, FVector EndLocation, FVector& Result);
public:
	APalAIController();

	EPathFollowingRequestResult::Type MoveToActor(AActor* TargetActor, float fAcceptanceRadius = 40.0f);
	EPathFollowingRequestResult::Type MoveToLocation(FVector TargetLocation, float fAcceptanceRadius = 400.0f);

	UFUNCTION(BlueprintPure, Category = "Pal AI Controller")
	bool GetMoveResult() const;

	void SetBBTargetActor(AActor* TargetActor);
	void SetBBTargetLocation(FVector TargetLocation);

	UFUNCTION(BlueprintCallable, Category = "Pal AI Controller")
	void ResetMove();
	
	UFUNCTION(BlueprintPure, Category = "Pal AI Controller")
	bool IsMoving() const { return bIsMove; }

	UFUNCTION(BlueprintPure, Category = "Pal AI Controller")
	FORCEINLINE FName GetBBTargetLocationName()  const{ return FName("TargetLocation"); }
	UFUNCTION(BlueprintPure, Category = "Pal AI Controller")
	FORCEINLINE FName GetBBTargetActorName()  const{ return FName("TargetActor"); }
};
