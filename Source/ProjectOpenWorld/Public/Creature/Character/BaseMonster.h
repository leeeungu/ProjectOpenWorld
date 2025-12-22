#pragma once

#include "CoreMinimal.h"
#include "GameBase/BaseCharacter.h"
#include "GameBase/Interface/AttackInterface.h"
#include "Pal/Interface/CommanderManageable.h"
#include "Pal/Interface/PalCommandInterface.h"
#include "BaseMonster.generated.h"

class UPalCommandComponent;
class UPalAttackComponent;

UCLASS()
class PROJECTOPENWORLD_API ABaseMonster : public ABaseCharacter , public IAttackInterface, public IPalCommandInterface, public ICommanderManageable
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
	float Hp{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
	float Attack{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
	float Defend{};

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr < UPalCommandComponent> CommandComponent{};

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr < UPalAttackComponent> AttackComponent{};

	bool bActionStarted{};
	bool bDead{};
public:
	ABaseMonster();


	virtual bool ReceiveCommand_Implementation(FPalCommand Command) override;
	UFUNCTION(BlueprintPure, Category = "CreatureAction")
	FORCEINLINE UPalAttackComponent* GetAttackComponent() const { return AttackComponent; }
	UFUNCTION(BlueprintPure, Category = "CreatureAction")
	UPalCommandComponent* GetCommandComponent() const { return CommandComponent; }
	 // //virtual bool Attacked_Implementation(IAttackInterface* Other) override;
	 // virtual float GetAttackValue_Implementation() const override;

	UFUNCTION(BlueprintPure, Category = "CreatureAction")
	bool GetActionStarted() const { return bActionStarted; }
	void SetActionStarted(bool bValue);
public:
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
};
