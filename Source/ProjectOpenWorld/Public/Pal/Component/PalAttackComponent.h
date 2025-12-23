#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Pal/Data/PalCommandData.h"
#include "PalAttackComponent.generated.h"

class UAnimSequence;
class APalAIController;
struct FAIRequestID;
namespace EPathFollowingResult { enum Type : int; }

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPalAttack);

USTRUCT(BlueprintType)
struct FPalAttackData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalAttackData")
	TObjectPtr<AActor> TargetActor{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalAttackData")
	ESubAttackType AttackSlot{};
};

USTRUCT(BlueprintType)
struct FPalTempAttackAnim
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalAttackData")
	TObjectPtr<UAnimSequence> Start{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalAttackData")
	TObjectPtr<UAnimSequence> Loop{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalAttackData")
	TObjectPtr<UAnimSequence> End{};
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UPalAttackComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "PalAttackData")
	FPalTempAttackAnim Current{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "PalAttackData")
	FPalTempAttackAnim Default{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "PalAttackData")
	FPalTempAttackAnim Skill01{};
	
	UPROPERTY()
	TObjectPtr<APalAIController> Controller{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalAttackData")
	float AttackDistance{};

	FPalAttackData AttackData{};
	bool bCanAttack{};
	bool bMoveStarted{};
	bool bAttacking{};
public:	
	UPROPERTY(BlueprintAssignable, Category = "PalAttackData")
	FOnPalAttack OnPalAttackEnd{};
	UPROPERTY(BlueprintAssignable, Category = "PalAttackData")
	FOnPalAttack OnPalAttackStart{};
	UPalAttackComponent();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void TargetIsDead(AActor* Actor);

	UFUNCTION()
	void FinishMove(FAIRequestID RequestID, EPathFollowingResult::Type Result);
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "PalAttackData")
	void  SetAttackData(FPalAttackData sData);
	UFUNCTION(BlueprintCallable, Category = "PalAttackData")
	void  StartAttack();
	UFUNCTION(BlueprintCallable, Category = "PalAttackData")
	void  EndAttack();
	FPalTempAttackAnim GetAnimation() const { return Current; }

	UFUNCTION(BlueprintPure, Category = "PalAttackData")
	bool GetAttacking() const { return bAttacking ; }

	UFUNCTION(BlueprintPure, Category = "PalAttackData")
	float GetAttackDistance() const { return AttackDistance; }
};
