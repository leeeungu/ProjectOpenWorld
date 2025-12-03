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

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UPalAttackComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "PalAttackData")
	UAnimSequence* StartAnim{};
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "PalAttackData")
	UAnimSequence* LoopAnim{};
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "PalAttackData")
	UAnimSequence* EndAnim{};

	FPalAttackData AttackData{};
public:	
	UPROPERTY(BlueprintAssignable, Category = "PalAttackData")
	FOnPalAttack OnPalAttackEnd{};
	UPROPERTY(BlueprintAssignable, Category = "PalAttackData")
	FOnPalAttack OnPalAttackStart{};
	UPalAttackComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "PalAttackData")
	void  SetAttackData(FPalAttackData sData);
	UFUNCTION(BlueprintCallable, Category = "PalAttackData")
	void  StartAttack();
	UFUNCTION(BlueprintCallable, Category = "PalAttackData")
	void  EndAttack();
};
