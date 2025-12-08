#pragma once

#include "CoreMinimal.h"
#include "Pal/CommandExecutor/PalCommandExecutorBase.h"
#include "PalCommandExecutor_Attack.generated.h"

class AActor;
class ABaseCreature;
class APalAIController;
struct FAIRequestID;
class UPalAttackComponent;
namespace EPathFollowingResult { enum Type : int; }

UCLASS()
class PROJECTOPENWORLD_API UPalCommandExecutor_Attack : public UPalCommandExecutorBase
{
	GENERATED_BODY()
protected:
	AActor* TargetActor{};
	APalAIController* OwnerController{};
	ABaseCreature* OwnerPal{};
	UPalAttackComponent* AttackComponent{};
	bool bStartedAttacking{};
public:
	virtual void Initialize(UPalCommandComponent* CommandComp) override;
	virtual void StartCommand(const FPalCommand& Command) override;
	virtual void Abort() override;

protected:
	UFUNCTION()
	void EndAttack();
};
