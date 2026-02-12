#pragma once

#include "CoreMinimal.h"
#include "GameBase/BaseCharacter.h"
#include "Pal/Interface/CommanderManageable.h"
#include "Pal/Interface/PalCommandInterface.h"
#include "Player/Interface/PlayerDetectInterface.h"
#include "BaseMonster.generated.h"

class UPalCommandComponent;
class UPalAttackComponent;
class UPalMonsterInteractionComponent;
struct FPalMonsterLevelData;
class UWidgetComponent;
class ABasePlayer;

UCLASS()
class PROJECTOPENWORLD_API ABaseMonster : public ABaseCharacter, public IPalCommandInterface, public ICommanderManageable, public IPlayerDetectInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Monster")
	FName MonsterName{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
	int Level{};

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr < UPalCommandComponent> PalCommand{};

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr < UPalAttackComponent> AttackComponent{};

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr <UPalMonsterInteractionComponent> MonsterInteractionComponent{};
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr <UWidgetComponent> HpWidgetComponent{};	
	bool bActionStarted{};
protected:
	virtual void BeginPlay() override;
public:
	ABaseMonster();


	virtual bool ReceiveCommand_Implementation(FPalCommand Command) override;
	UFUNCTION(BlueprintPure, Category = "CreatureAction")
	FORCEINLINE UPalAttackComponent* GetAttackComponent() const { return AttackComponent; }
	UFUNCTION(BlueprintPure, Category = "CreatureAction")
	FORCEINLINE UPalCommandComponent* GetCommandComponent() const { return PalCommand; }
	UFUNCTION(BlueprintPure, Category = "CreatureAction")
	FORCEINLINE UPalMonsterInteractionComponent* GetMonsterInteractionComponent() const { return MonsterInteractionComponent; }

	 // //virtual bool Attacked_Implementation(IAttackInterface* Other) override;
	 // virtual float GetAttackValue_Implementation() const override;

	UFUNCTION(BlueprintPure, Category = "CreatureAction")
	bool GetActionStarted() const { return bActionStarted; }
	void SetActionStarted(bool bValue);
	virtual void PossessedBy(AController* NewController) override;
public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Damaged")
	FOnDamageedDelegate OnDamagedDelegate{};
	void SetPalMonsterLevelData(int lv, const FPalMonsterLevelData& LevelData);

public:
	int GetMonsterLevel() const { return Level; }
	float GetCurrentHp() const;
	float GetMaxHp() const;
	FName GetMonsterName() const { return MonsterName; }
	//AttackInterface
	virtual float GetAttackValue_Implementation() const override;
	virtual void  SetAttackValue_Implementation(float NewValue) override;
	virtual void  RetAttackValue_Implementation() override;
	virtual bool DamagedCharacter_Implementation(const TScriptInterface< IAttackInterface>& Other) override;
	virtual bool IsDead_Implementation() const;


	virtual EPalCommandKind GetCommandKind_Implementation() override;
	virtual uint8 GetSubCommandType_Implementation() override;
	virtual FPalCommand GetCommand_Implementation() override;
	virtual bool IsCommandFinished_Implementation() override;

	// IPlayerDetectInterface
	virtual void OnDetectBeginEvent_Implementation(ABasePlayer* Player)override;
	virtual void OnDetectEndEvent_Implementation(ABasePlayer* Player)override;

};
