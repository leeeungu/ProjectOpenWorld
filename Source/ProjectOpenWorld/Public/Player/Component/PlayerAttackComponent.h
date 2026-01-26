#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerAttackComponent.generated.h"

UENUM(BlueprintType)
enum class EPlayerAttackType : uint8
{
	None UMETA(Hidden),
	Default,
	Skill01,
	Skill02,
	Skill03,
	KnockBack,
	Hit01,
	Hit02,
	Hit03,
	Dead,
	EnumMax UMETA(Hidden)
};
class UAnimMontage;
class UBaseAnimInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerAttackEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayMontage, EPlayerAttackType, NewType);

USTRUCT(BlueprintType)
struct FPlayerAttackData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerAttackData")
	EPlayerAttackType PlayerAttackType{ EPlayerAttackType::Default };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerAttackData")
	TArray<TObjectPtr<UAnimMontage>>  AttackMontageArray{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerAttackData")
	float CoolTime{ 1.0f };
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UPlayerAttackComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerAttack")
	TObjectPtr<UDataTable> PlayerAttackDataTable{};

	TArray< FPlayerAttackData*> PlayerAttackDataArray{};
	TArray< float > MontageCoolTime{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerAttack")
	EPlayerAttackType CurrentAttackType{ EPlayerAttackType::None };

	TObjectPtr < UBaseAnimInstance> AnimInstance{};

	bool bIsAttacking{ false };
public:	
	UPlayerAttackComponent();
	FOnPlayerAttackEnd OnPlayerAttackEnd{};
	FOnPlayMontage OnPlayMontage{};
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnAttackMontageEnd();

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "PlayerAttack")
	void Attack(EPlayerAttackType InAttackType);

	void StopAttack();
	float GetMontageCoolTime(EPlayerAttackType InAttackType) const;
	bool IsAttacking() const { return bIsAttacking; }
	bool IsHitted() const { return CurrentAttackType == EPlayerAttackType::Hit01 || CurrentAttackType == EPlayerAttackType::Hit02 || CurrentAttackType == EPlayerAttackType::Hit03; }
};
