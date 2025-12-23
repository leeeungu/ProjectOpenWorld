#pragma once

#include "CoreMinimal.h"
#include "Pal/CommandExecutor/PalCommandExecutorBase.h"
#include "PalCommandExecutor_MonsterAttack.generated.h"

class AActor;
class ABaseMonster;
class APalAIController;
struct FAIRequestID;
class UPalAttackComponent;
namespace EPathFollowingResult { enum Type : int; }

UCLASS()
class PROJECTOPENWORLD_API UPalCommandExecutor_MonsterAttack : public UPalCommandExecutorBase
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	TObjectPtr < AActor> TargetActor{};
	UPROPERTY()
	TObjectPtr<ABaseMonster> OwnerPal{};
	UPROPERTY()
	TObjectPtr < UPalAttackComponent> AttackComponent{};
	bool bStartedAttacking{};
public:
	virtual void Initialize(UPalCommandComponent* CommandComp) override;
	virtual bool StartCommand(const FPalCommand& Command) override;
	virtual void Abort() override;

protected:
	UFUNCTION()
	void EndAttack();
};
