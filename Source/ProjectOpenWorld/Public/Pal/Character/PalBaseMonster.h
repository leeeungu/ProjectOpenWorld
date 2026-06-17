#pragma once

#include "CoreMinimal.h"
#include "Pal/Character/PalBaseCharacter.h"
#include "Player/Interface/PlayerDetectInterface.h"
#include "GameBase/Interface/AttackInterface.h"
#include "PalBaseMonster.generated.h"

class UPalPatrolComponent;
class UPalCommandComponent;
class UPalAttackComponent;
struct FPalMonsterLevelData;
class UWidgetComponent;
class ABasePlayer;
class UPalHitHandlerComponent;
class UStatComponent;
class UPalMonsterCombatComponent;

UCLASS()
class PROJECTOPENWORLD_API APalBaseMonster : public APalBaseCharacter, 
	public IAttackInterface, public IPlayerDetectInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Monster")
	FName MonsterName{};
	UPROPERTY(EditAnywhere, Category = "Monster")
	int Level{};

	// attack관련 componnet 공격 타입 관리 공격 중 관리
	UPROPERTY( VisibleAnywhere)
	TObjectPtr < UPalAttackComponent> AttackComponent{};

	// hit 관련 component
	UPROPERTY( VisibleAnywhere)
	TObjectPtr < UPalHitHandlerComponent> HitHandlerComponent{};

	UPROPERTY( VisibleAnywhere)
	TObjectPtr <UPalMonsterCombatComponent> MonsterCombatComponent{};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pal|Component")
	TObjectPtr<UPalPatrolComponent> PalPatrolComponent{};
	FTimerHandle Deadhandle{};
public:
	APalBaseMonster();
	void InitializeLevel(int32 nLevel);

	UFUNCTION()
	void OnMoveSpeedChanged(double PreCurrentStat, double CurrentStat);

	UFUNCTION(BlueprintPure, Category = "Pal|Component")
	FORCEINLINE UPalPatrolComponent* GetPalPatrolComponent() const { return PalPatrolComponent; }
	UFUNCTION(BlueprintPure, Category = "Pal|Component")
	FORCEINLINE UPalAttackComponent* GetAttackComponent() const { return AttackComponent; }
	UFUNCTION(BlueprintPure, Category = "Pal|Component")
	int GetMonsterLevel() const { return Level; }
	virtual FName GetPalName() const override { return MonsterName; }

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


	// iattackinterface
	virtual float GetAttackValue_Implementation() const override;
	virtual bool IsDead_Implementation() const override;
	virtual float GetAttackDistance() const override;
	virtual void  SetAttackValue_Implementation(float NewValue) override {}
	virtual void  RetAttackValue_Implementation() override {}
	virtual bool DamagedCharacter_Implementation(const TScriptInterface< IAttackInterface>& Other) override { return true; }
	virtual UPalHitHandlerComponent* GetHitHandlerComponent() const override { return HitHandlerComponent; }
	virtual UPalAttackComponent* GetPalAttackComponent() const { return AttackComponent; }

	// IPlayerDetectInterface
	virtual void OnDetectBeginEvent_Implementation(ABasePlayer* Player)override;
	virtual void OnDetectEndEvent_Implementation(ABasePlayer* Player)override;
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHPChanged(double PreCurrentStat, double CurrentStat);
};
