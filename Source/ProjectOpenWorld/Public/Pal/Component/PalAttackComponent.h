#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Pal/Data/PalCommandData.h"
#include "GameBase/Class/TimerClass.h"
#include "Engine/DataTable.h"
#include "GameBase/Interface/MontageQueueInterface.h"
#include "PalAttackComponent.generated.h"

class UAnimSequence;
class UAnimMontage;
class APalAIController;
struct FAIRequestID;
class ABaseCharacter;
class UDataAsset;
namespace EPathFollowingResult { enum Type : int; }

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPalAttack);

USTRUCT(BlueprintType)
struct FPalAttackDataTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalAttackData")
	ESubAttackType AttackSlot{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalAttackData")
	TArray<TObjectPtr<UAnimMontage>> AttackData{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalAttackData")
	float AttackDistance = 100.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalAttackData")
	float AttackCooldown = 5.0f;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UPalAttackComponent : public UActorComponent //, public IMontageQueueInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "PalAttackData")
	FPalAttackDataTable AttackData{};

	TArray< FPalAttackDataTable*> AllAttackDataArray{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalAttackData")
	TWeakObjectPtr<AActor> TargetActor{};
	UPROPERTY()
	TObjectPtr< ABaseCharacter> OwnerCharacter{};
	UPROPERTY()
	TObjectPtr<APalAIController> Controller{};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "PalAttackData")
	TObjectPtr<UDataTable> AttackDataAsset{};
	int AttackIndex{};
	UPROPERTY(VisibleAnywhere, Category = "PalAttackData")
	bool bSetTargetData{};
	UPROPERTY(VisibleAnywhere, Category = "PalAttackData")
	bool bAttacking{};
	UPROPERTY(VisibleAnywhere, Category = "PalAttackData")
	bool bSetAttackData{};

	TArray<bool> CoolDownArray{};
public:	
	UPROPERTY(BlueprintAssignable, Category = "PalAttackData")
	FOnPalAttack OnPalAttackEnd{};
	UPROPERTY(BlueprintAssignable, Category = "PalAttackData")
	FOnPalAttack OnPalAttackStart{};
	UPalAttackComponent();

protected:
	virtual void BeginPlay() override;

	void ResetAttackData();
public:	
	void ResetAttack();

	UFUNCTION(BlueprintCallable, Category = "PalAttackData")
	void  SetAttackTarget(AActor* Actor);
	UFUNCTION(BlueprintCallable, Category = "PalAttackData")
	void  SetAttackData(ESubAttackType eType);
	UFUNCTION(BlueprintCallable, Category = "PalAttackData")
	void  StartAttack();
	UFUNCTION(BlueprintCallable, Category = "PalAttackData")
	void  EndAttack();
	UFUNCTION(BlueprintCallable, Category = "PalAttackData")
	void StopAttack();

	UFUNCTION(BlueprintPure, Category = "PalAttackData")
	bool TargetIsInRange() const;
	UFUNCTION(BlueprintPure, Category = "PalAttackData")
	bool GetAttacking() const { return bAttacking ; }

	UFUNCTION(BlueprintPure, Category = "PalAttackData")
	float GetAttackDistance() const { return AttackData.AttackDistance; }
	UFUNCTION(BlueprintPure, Category = "PalAttackData")
	bool IsSetTarget() const { return bSetTargetData; }
	UFUNCTION(BlueprintPure, Category = "PalAttackData")
	bool IsAttacking() const { return bAttacking; }

	UFUNCTION(BlueprintPure, Category = "PalAttackData")
	AActor* GetTargetActor() const;
	UFUNCTION(BlueprintPure, Category = "PalAttackData")
	bool IsSetAttackData()const { return bSetAttackData; }

	bool IsTargetNotDead() const;
	bool IsCoolDown(ESubAttackType Type) const;
	//// IMontageQueueInterface을(를) 통해 상속됨
	//UAnimMontage* GetMontage() const override;
	//void MontageStartEvent(UBaseAnimInstance* BaseAnim, UAnimMontage* Montage) override;
	//void MontageBlendingEvent(UBaseAnimInstance* BaseAnim, UAnimMontage* Montage, bool bInterrupted) override;
};
