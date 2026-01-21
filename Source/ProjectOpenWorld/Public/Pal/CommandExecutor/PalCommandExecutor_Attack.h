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
	UPROPERTY()
	AActor* TargetActor{};
	UPROPERTY()
	ABaseCreature* OwnerPal{};
	UPROPERTY()
	UPalAttackComponent* AttackComponent{};

	UPROPERTY()
	APalAIController* OwnerController{};
	bool bStartedAttacking{};
public:
	virtual void Initialize(UPalCommandComponent* CommandComp) override;
	virtual bool StartCommand(const FPalCommand& Command) override;
	virtual void Abort() override;
	virtual void WorkCommand() override;
	virtual bool CheckCommandValid() override;

protected:
	UFUNCTION()
	void EndAttack();
};
