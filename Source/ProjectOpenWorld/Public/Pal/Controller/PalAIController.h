#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h" 
#include "PalAIController.generated.h"

UCLASS()
class PROJECTOPENWORLD_API APalAIController : public AAIController
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TObjectPtr<UBehaviorTree> BTree{};

	bool bIsMove{};
protected:
	virtual void BeginPlay() override;
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
	void ResetMove();
	
	UFUNCTION(BlueprintPure, Category = "Pal AI Controller")
	bool IsMoving() const { return bIsMove; }

	FORCEINLINE static FName GetBBTargetLocationName() { return FName("TargetLocation"); }
	FORCEINLINE static FName GetBBTargetActorName() { return FName("TargetActor"); }
};
