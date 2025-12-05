#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PalAttackComponent.generated.h"


class UAnimSequence;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPalAttack);

USTRUCT(BlueprintType)
struct FPalAttackData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalAttackData")
	AActor* TargetActor{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalAttackData")
	uint8 AttackSlot{};
};

USTRUCT(BlueprintType)
struct FPalTempAttackAnim
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalAttackData")
	UAnimSequence* Start{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalAttackData")
	UAnimSequence* Loop{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PalAttackData")
	UAnimSequence* End{};
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


	FPalAttackData AttackData{};
	bool bCanAttack{};
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
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "PalAttackData")
	void  SetAttackData(FPalAttackData sData);
	UFUNCTION(BlueprintCallable, Category = "PalAttackData")
	void  StartAttack();
	UFUNCTION(BlueprintCallable, Category = "PalAttackData")
	void  EndAttack();
	FPalTempAttackAnim GetAnimation() const { return Current; }
};
