#pragma once

#include "CoreMinimal.h"
#include "GameBase/BaseCharacter.h"
#include "Creature/Interface/CreatureActionInterface.h"
#include "Creature/Interface/CreatureMessageInterface.h"
#include "Creature/Interface/CreatureAttackInterface.h"
#include "BaseCreature.generated.h"

struct FAIRequestID;
namespace EPathFollowingResult { enum Type : int; }

UCLASS()
class PROJECTOPENWORLD_API ABaseCreature : public ABaseCharacter, public ICreatureMessageInterface, public ICreatureAttackInterface
{
	GENERATED_BODY()
protected:
	TScriptInterface<ICreatureActionInterface> ActionComponents[(uint8)ECreatureActionType::Action_Max]{};
		
	UPROPERTY(EditDefaultsOnly, Meta = (Bitmask, BitmaskEnum = "ECreatureRollType"))
	int32 CreatureRoll{};

	ECreatureActionType ActionType{};
	ECreatureActionType NextActionType{};
	TSoftObjectPtr<UObject> TargetObj{};
protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void FinishActionMove(FAIRequestID RequestID, EPathFollowingResult::Type Result);
	bool MoveToTarget();
public:
	virtual void ReceiveMessage_Implementation(EMessageType MessageType, AActor* SendActor, UObject* TargetObject = nullptr) override;
	virtual void ReceiveActionMessage_Implementation(ECreatureActionType MessageType, AActor* SendActor, UObject* TargetObject = nullptr) override;
	virtual float GetAttackDamage_Implementation() const override;

	UFUNCTION(BlueprintPure, Category = "CreatureAction")
	FORCEINLINE ECreatureActionType GetCreatureActionType() const { return ActionType; }
	void ResetAction();
};
