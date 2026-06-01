#pragma once

#include "CoreMinimal.h"
#include "GameBase/BaseCharacter.h"
#include "Pal/Interface/CommanderManageable.h"
#include "Pal/Interface/PalCommandInterface.h"
#include "Player/Interface/PlayerDetectInterface.h"
#include "BaseMonster.generated.h"

class UPalCommandComponent;
class UPalAttackComponent;
struct FPalMonsterLevelData;
class UWidgetComponent;
class ABasePlayer;
class UPalHitHandlerComponent;

// APalBaseMonster 로이전으로 인한 메크로 설정
UCLASS(
	Abstract,                  // SpawnActor / NewObject 불가
	NotBlueprintable,          // BP 자식 클래스 못 만듦
	NotPlaceable,              // 레벨 드래그 배치 못함
	HideDropdown,              // 클래스 드롭다운에서 숨김
	meta = (
		DeprecationMessage = "Use APalBaseMonster instead. This class will be removed."
		)
)
class  PROJECTOPENWORLD_API ABaseMonster : public ABaseCharacter, public IPalCommandInterface, public ICommanderManageable, public IPlayerDetectInterface
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
	TObjectPtr < UPalHitHandlerComponent> HitHandlerComponent{};
	
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

	 // //virtual bool Attacked_Implementation(IAttackInterface* Other) override;
	 // virtual float GetAttackValue_Implementation() const override;

	UFUNCTION(BlueprintPure, Category = "CreatureAction")
	bool GetActionStarted() const { return bActionStarted; }
	void SetActionStarted(bool bValue);
	virtual void PossessedBy(AController* NewController) override;
	UFUNCTION()
	void SpawnItem();

public:
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
	virtual UPalHitHandlerComponent* GetHitHandlerComponent() const { return HitHandlerComponent; }

	virtual EPalCommandKind GetCommandKind_Implementation() override;
	virtual uint8 GetSubCommandType_Implementation() override;
	virtual FPalCommand GetCommand_Implementation() override;
	virtual bool IsCommandFinished_Implementation() override;

	// IPlayerDetectInterface
	virtual void OnDetectBeginEvent_Implementation(ABasePlayer* Player)override;
	virtual void OnDetectEndEvent_Implementation(ABasePlayer* Player)override;

};
