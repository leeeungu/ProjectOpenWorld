#pragma once

#include "CoreMinimal.h"
#include "GameBase/BaseCharacter.h"
#include "GameBase/Interface/AttackInterface.h"
#include "Creature/Interface/CreatureActionInterface.h"
#include "Creature/Interface/CreatureMessageInterface.h"
#include "Pal/Component/PalAllyCommandComponent.h"
#include "BaseCreature.generated.h"

struct FAIRequestID;
namespace EPathFollowingResult { enum Type : int; }

UCLASS()
class PROJECTOPENWORLD_API ABaseCreature : public ABaseCharacter, public ICreatureMessageInterface,  public IAttackInterface
{
	GENERATED_BODY()
protected:
	TScriptInterface<ICreatureActionInterface> ActionComponents[(uint8)ECreatureActionType::Action_Max]{};
		
	UPROPERTY(EditDefaultsOnly, Meta = (Bitmask, BitmaskEnum = "ECreatureRollType"))
	int32 CreatureRoll{};

	ECreatureActionType ActionType{};
	ECreatureActionType NextActionType{};

	TSoftObjectPtr<AActor> ActionFrom{};
	TSoftObjectPtr<AActor> TargetActor{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
	float Hp{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
	float Attack{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
	float Defend{};
protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void FinishActionMove(FAIRequestID RequestID, EPathFollowingResult::Type Result);
	bool MoveToTarget();
public:
	virtual void ReceiveMessage_Implementation(EMessageType MessageType, AActor* SendActor, AActor* TargetObject = nullptr) override;
	virtual void ReceiveActionMessage_Implementation(ECreatureActionType MessageType, AActor* SendActor, AActor* TargetObject = nullptr) override;

	UFUNCTION(BlueprintPure, Category = "CreatureAction")
	bool GetIsActionStarted(ECreatureActionType Type);

	UFUNCTION(BlueprintPure, Category = "CreatureAction")
	FORCEINLINE ECreatureActionType GetCreatureActionType() const { return ActionType; }
	void ResetAction();


	void ResetActionMode();
	void TransportActionMode();

public:
	//AttackInterface
	virtual float GetAttackValue_Implementation() const override;
	virtual void  SetAttackValue_Implementation(float NewValue) override;
	virtual void  RetAttackValue_Implementation() override;
	virtual bool DamagedCharacter_Implementation(const TScriptInterface< IAttackInterface>& Other) override;
};
