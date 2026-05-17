#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Pal/Interface/CommanderManageable.h"
#include "Pal/Interface/PalCommandInterface.h"
#include "Player/Interface/PlayerDetectInterface.h"
#include "GameBase/Interface/AttackInterface.h"
#include "PalBaseCharacter.generated.h"

class UPalCommandComponent;
class UPalAttackComponent;
struct FPalMonsterLevelData;
class UWidgetComponent;
class ABasePlayer;
class UPalHitHandlerComponent;
class UStatComponent;

UCLASS()
class PROJECTOPENWORLD_API APalBaseCharacter : public ACharacter
{
	GENERATED_BODY()
protected:

protected:
	//virtual void BeginPlay() override;
public:
	//APalBaseCharacter();


//	virtual bool ReceiveCommand_Implementation(FPalCommand Command) override;
//	UFUNCTION(BlueprintPure, Category = "CreatureAction")
//	FORCEINLINE UPalAttackComponent* GetAttackComponent() const { return AttackComponent; }
//	UFUNCTION(BlueprintPure, Category = "CreatureAction")
//	FORCEINLINE UPalCommandComponent* GetCommandComponent() const { return PalCommand; }
//
//	// //virtual bool Attacked_Implementation(IAttackInterface* Other) override;
//	// virtual float GetAttackValue_Implementation() const override;
//
//	UFUNCTION(BlueprintPure, Category = "CreatureAction")
//	bool GetActionStarted() const { return bActionStarted; }
//	void SetActionStarted(bool bValue);
//	virtual void PossessedBy(AController* NewController) override;
//	UFUNCTION()
//	void SpawnItem();
//
//public:
//	void SetPalMonsterLevelData(int lv, const FPalMonsterLevelData& LevelData);
//
//public:
//	
//	
//	float GetCurrentHp() const;
//	float GetMaxHp() const;
//	//AttackInterface
//	virtual float GetAttackValue_Implementation() const override;
//	virtual void  SetAttackValue_Implementation(float NewValue) override;
//	virtual void  RetAttackValue_Implementation() override;
//	virtual bool DamagedCharacter_Implementation(const TScriptInterface< IAttackInterface>& Other) override;
//	virtual bool IsDead_Implementation() const;
//	virtual UPalHitHandlerComponent* GetHitHandlerComponent() const { return HitHandlerComponent; }
//
//	virtual EPalCommandKind GetCommandKind_Implementation() override;
//	virtual uint8 GetSubCommandType_Implementation() override;
//	virtual FPalCommand GetCommand_Implementation() override;
//	virtual bool IsCommandFinished_Implementation() override;
//
//	// IPlayerDetectInterface
//	virtual void OnDetectBeginEvent_Implementation(ABasePlayer* Player)override;
//	virtual void OnDetectEndEvent_Implementation(ABasePlayer* Player)override;

};
