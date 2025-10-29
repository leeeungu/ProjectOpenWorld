#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CreatureAttackComponent.generated.h"

class UAnimMontage;
class ACharacter;

USTRUCT(BlueprintType)
struct FCreatureAttackData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "AttackData")
	TSoftObjectPtr<UAnimMontage> Montage{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "AttackData")
	float AttackDelay{};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "AttackData")
	float AttackDamage = 1;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UCreatureAttackComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	TArray<FCreatureAttackData> ArrayMontage{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
	int MontageIndex{};

	TSoftObjectPtr < ACharacter> OwnerCharacter{};
public:	
	UCreatureAttackComponent();

	void SetAttackIndex(int montageIndex);
	int GetAttackIndex() const { return MontageIndex;	}
	void PlayAttackMontage(int montageIndex = - 1) const;
	bool GetAttackData(int montageIndex, FCreatureAttackData& Data) const;

	UFUNCTION(BlueprintPure, Category = "Attack")
	float GetDamage() const;
protected:
	virtual void BeginPlay() override;

public:	
//	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
