#pragma once

#include "CoreMinimal.h"
#include "GameBase/BaseCharacter.h"
#include "Creature/Interface/CreatureActionInterface.h"
#include "Creature/Interface/CreatureMessageInterface.h"
#include "Creature/Interface/CreatureAttackInterface.h"
#include "BaseCreature.generated.h"

class UCreatureAttackComponent;

UCLASS()
class PROJECTOPENWORLD_API ABaseCreature : public ABaseCharacter, public ICreatureMessageInterface, public ICreatureAttackInterface
{
	GENERATED_BODY()
protected:
	TScriptInterface<ICreatureActionInterface> ActionComponents[(uint8)ECreatureActionType::Action_Max]{};
	
	UPROPERTY(EditDefaultsOnly, Meta = (Bitmask, BitmaskEnum = "ECreatureRollType"))
	int32 CreatureRoll{};

	ECreatureActionType ActionType{};

	TSoftObjectPtr< UCreatureAttackComponent> AttackComponent{};
protected:
	virtual void BeginPlay() override;
public:
	virtual void ReceiveMessage_Implementation(EMessageType MessageType, AActor* SendActor, UObject* TargetObject = nullptr) override;

	virtual UCreatureAttackComponent* GetAttackComponent_Implementation() const override;
	virtual float GetAttackDamage_Implementation() const override;

	UFUNCTION(BlueprintPure, Category = "CreatureAction")
	FORCEINLINE ECreatureActionType GetCreatureActionType() const { return ActionType; }
};
