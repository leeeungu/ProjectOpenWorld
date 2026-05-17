#pragma once

#include "CoreMinimal.h"
#include "Pal/Character/PalBaseCharacter.h"
#include "GameBase/Interface/AttackInterface.h"
#include "PalBaseCreature.generated.h"

class UPalPatrolComponent;
class UPalCommandComponent;
class UPalAttackComponent;
class UPalHitHandlerComponent;
class UStatComponent;

UCLASS(Category = "PalCreature")
class PROJECTOPENWORLD_API APalBaseCreature : public APalBaseCharacter, public IAttackInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "PalCreature")
	FName PalName{};
	UPROPERTY(EditAnywhere, Category = "PalCreature")
	int Level{};

	//hp, attack defend 같은 statut 관련 component
	UPROPERTY(VisibleAnywhere, Category = "PalCreature")
	TObjectPtr< UStatComponent> StatComponent{};

	// attack관련 componnet 공격 타입 관리 공격 중 관리
	UPROPERTY(VisibleAnywhere, Category = "PalCreature")
	TObjectPtr < UPalAttackComponent> AttackComponent{};

	// hit 관련 component
	UPROPERTY(VisibleAnywhere, Category = "PalCreature")
	TObjectPtr < UPalHitHandlerComponent> HitHandlerComponent{};

	UPROPERTY(VisibleAnywhere, Category = "PalCreature")
	TObjectPtr < UPalJobComponent> JobComponent{};

public:
	APalBaseCreature();

	TObjectPtr<UPalPatrolComponent> PalPatrolComponent{};
public:
	UFUNCTION(BlueprintPure, Category = "Pal|Component")
	FORCEINLINE UPalAttackComponent* GetAttackComponent() const { return AttackComponent; }
	UFUNCTION(BlueprintPure, Category = "Pal|Component")
	FORCEINLINE UPalJobComponent* GetJobComponent() const { return JobComponent; }
	
	UFUNCTION(BlueprintPure, Category = "Pal|Component")
	int GetPalLevel() const { return Level; }
	UFUNCTION(BlueprintPure, Category = "Pal|Component")
	FName GetPalName() const { return PalName; }



	// iattackinterface
	virtual float GetAttackValue_Implementation() const override;
	virtual bool IsDead_Implementation() const override;
	virtual void  SetAttackValue_Implementation(float NewValue) override {}
	virtual void  RetAttackValue_Implementation() override {}
	virtual bool DamagedCharacter_Implementation(const TScriptInterface< IAttackInterface>& Other) override { return true; }
	virtual UPalHitHandlerComponent* GetHitHandlerComponent() const override { return HitHandlerComponent; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void HPChanged(double PreCurrentStat, double CurrentStat);
};