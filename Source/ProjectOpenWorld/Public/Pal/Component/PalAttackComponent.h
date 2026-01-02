#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Pal/Data/PalCommandData.h"
#include "GameBase/Class/TimerClass.h"
#include "GameBase/Interface/MontageQueueInterface.h"
#include "PalAttackComponent.generated.h"

class UAnimSequence;
class UAnimMontage;
class APalAIController;
struct FAIRequestID;
class ABaseCharacter;
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
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalAttackData")
	TArray<TObjectPtr<UAnimMontage>> AttackData{};
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UPalAttackComponent : public UActorComponent, public IMontageQueueInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "PalAttackData")
	FPalAttackData AttackData{};
	UPROPERTY()
	TObjectPtr< ABaseCharacter> OwnerCharacter{};
	UPROPERTY()
	TObjectPtr<APalAIController> Controller{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalAttackData")
	float AttackDistance{};

	int AttackIndex{};
	bool bSetTargetData{};
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
public:	
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "PalAttackData")
	void  SetAttackData(FPalAttackData sData);
	UFUNCTION(BlueprintCallable, Category = "PalAttackData")
	void  StartAttack();
	UFUNCTION(BlueprintCallable, Category = "PalAttackData")
	void  EndAttack();

	UFUNCTION(BlueprintPure, Category = "PalAttackData")
	bool GetAttacking() const { return bAttacking ; }

	UFUNCTION(BlueprintPure, Category = "PalAttackData")
	float GetAttackDistance() const { return AttackDistance; }
	UFUNCTION(BlueprintPure, Category = "PalAttackData")
	bool IsSetTarget() const { return bSetTargetData; }

	// IMontageQueueInterface을(를) 통해 상속됨
	UAnimMontage* GetMontage() const override;
	void MontageStartEvent(UBaseAnimInstance* BaseAnim, UAnimMontage* Montage) override;
	void MontageBlendingEvent(UBaseAnimInstance* BaseAnim, UAnimMontage* Montage, bool bInterrupted) override;
};
