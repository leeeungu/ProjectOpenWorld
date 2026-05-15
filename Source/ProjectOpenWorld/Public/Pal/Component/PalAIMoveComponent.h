#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PalAIMoveComponent.generated.h"

class AAIController;
class UBlackboardComponent;
struct FAIRequestID;
struct FAIMoveRequest;
namespace EPathFollowingResult { enum Type : int; }

UENUM(BlueprintType)
enum class EPalMoveStatus : uint8
{
	Idle    UMETA(DisplayName = "Idle"),
	Moving  UMETA(DisplayName = "Moving"),
	Arrived UMETA(DisplayName = "Arrived"),
	Failed  UMETA(DisplayName = "Failed"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPalMoveStatusChanged, EPalMoveStatus, OldStatus, EPalMoveStatus, NewStatus);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UPalAIMoveComponent : public UActorComponent
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleAnywhere, Category = "PalAIMovement")
	TObjectPtr<AAIController> OwnerAIController{};
	UPROPERTY(VisibleAnywhere, Category = "PalAIMovement")
	TWeakObjectPtr<AActor> TargetActor{};
	UPROPERTY(VisibleAnywhere, Category = "PalAIMovement")
	FVector TargetLocation{};
	UPROPERTY(VisibleAnywhere, Category = "PalAIMovement")
	float ActorAcceptanceRadius{};
	UPROPERTY(VisibleAnywhere, Category = "PalAIMovement")
	float LocationAcceptanceRadius{};
	UPROPERTY(VisibleAnywhere, Category = "PalAIMovement")
	EPalMoveStatus MoveStatus{ EPalMoveStatus::Idle };

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:
	UFUNCTION()
	void TargetDestroyed(AActor* DestroyedActor);
	UFUNCTION()
	void MoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);
	void SetStatus(EPalMoveStatus NewStatus);
public:	
	UPROPERTY(BlueprintAssignable, Category = "PalAIMovement")
	FOnPalMoveStatusChanged OnMoveStatusChanged{};

	UFUNCTION(BlueprintCallable, Category = "PalAIMovement")
	void StopMove();
	UFUNCTION(BlueprintCallable, Category = "PalAIMovement")
	void ResetMove();

	UFUNCTION(BlueprintCallable, Category = "PalAIMovement")
	void SetTargetActor(AActor* Target, float fAcceptanceRadius = 200.0f);
	UFUNCTION(BlueprintCallable, Category = "PalAIMovement")
	bool MoveToTargetActor();
	UFUNCTION(BlueprintPure, Category = "PalAIMovement")
	AActor* GetTargetActor() const;
	UFUNCTION(BlueprintCallable, Category = "PalAIMovement")
	void ResetTargetActor();


	UFUNCTION(BlueprintCallable, Category = "PalAIMovement")
	void SetTargetLocation(FVector Target, float fAcceptanceRadius = 200.0f);
	UFUNCTION(BlueprintCallable, Category = "PalAIMovement")
	bool MoveToTargetLocation();
	UFUNCTION(BlueprintPure, Category = "PalAIMovement")
	bool GetTargetLocation(FVector& Target) const;
	UFUNCTION(BlueprintCallable, Category = "PalAIMovement")
	void ResetTargetLocation();

	UFUNCTION(BlueprintPure, Category = "PalAIMovement")
	FORCEINLINE bool IsMoving() const { return MoveStatus == EPalMoveStatus::Moving; }
	UFUNCTION(BlueprintPure, Category = "PalAIMovement")
	FORCEINLINE bool IsArrived() const { return MoveStatus == EPalMoveStatus::Arrived; }
	UFUNCTION(BlueprintPure, Category = "PalAIMovement")
	FORCEINLINE EPalMoveStatus GetMoveStatus() const { return MoveStatus; }
};
