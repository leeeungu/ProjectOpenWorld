#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Pal/Data/PalCommandData.h"
#include "GameBase/Class/TimerClass.h"
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
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "PalAttackData")
	FPalAttackData AttackData{};
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "PalAttackData")
	TObjectPtr<UAnimMontage> Curent{};
	UPROPERTY()
	TObjectPtr< ABaseCharacter> OwnerCharacter{};
	UPROPERTY()
	TObjectPtr<APalAIController> Controller{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalAttackData")
	float AttackDistance{};

	TimerClass Timer{};

	int AttackIndex{};
	bool bSetTargetData{};
	bool bAttacking{};
	float AttackTime{};
	bool ChangeAnim{};
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
	void PlayNextAttack( UAnimMontage* Montage, bool bInterrupted);
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "PalAttackData")
	void  SetAttackData(FPalAttackData sData);
	UFUNCTION(BlueprintCallable, Category = "PalAttackData")
	void  StartAttack();
	UFUNCTION(BlueprintCallable, Category = "PalAttackData")
	void  EndAttack();

	//UFUNCTION(BlueprintPure, Category = "PalAttackData")
	//UAnimMontage* GetAttackAnimation() const { return Curent; }
	UFUNCTION(BlueprintPure, Category = "PalAttackData")
	bool IsChangeAnim() const { return ChangeAnim; }
	UFUNCTION(BlueprintPure, Category = "PalAttackData")
	bool GetAttacking() const { return bAttacking ; }

	UFUNCTION(BlueprintPure, Category = "PalAttackData")
	float GetAttackDistance() const { return AttackDistance; }
	UFUNCTION(BlueprintPure, Category = "PalAttackData")
	bool IsSetTarget() const { return bSetTargetData; }
	//UFUNCTION(BlueprintPure, Category = "PalAttackData")
	//bool GetCanRotate() const { return bCanRotate; }
	//UFUNCTION(BlueprintCallable, Category = "PalAttackData")
	//void SetCanRotate(bool Value) { bCanRotate = Value; }
};
