#pragma once

#include "CoreMinimal.h"
#include "Pal/CommandExecutor/PalCommandExecutorBase.h"
#include "PalCommandExecutor_Mining.generated.h"

class AActor;
class ABaseCreature;
class APalAIController;
struct FAIRequestID;
namespace EPathFollowingResult { enum Type : int; }

UCLASS()
class PROJECTOPENWORLD_API UPalCommandExecutor_Mining : public UPalCommandExecutorBase
{
	GENERATED_BODY()
protected:
	AActor* TargetResource{};
	APalAIController* OwnerController{};
	ABaseCreature* OwnerPal{};
	bool bStartedMining{};
public:
	virtual void Initialize(UPalCommandComponent* CommandComp) override;
	virtual void StartCommand(const FPalCommand& Command) override;
	virtual void Abort() override;

protected:
	UFUNCTION()
	void EndMining();

	UFUNCTION()
	void FinishMove(FAIRequestID RequestID, EPathFollowingResult::Type Result);
};
