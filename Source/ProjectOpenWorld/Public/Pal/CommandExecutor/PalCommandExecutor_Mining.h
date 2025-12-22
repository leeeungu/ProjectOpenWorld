#pragma once

#include "CoreMinimal.h"
#include "Pal/CommandExecutor/PalCommandExecutorBase.h"
#include "PalCommandExecutor_Mining.generated.h"

class AActor;
class ABaseCreature;
class APalAIController;
struct FAIRequestID;
class UInteractionComponent;
namespace EPathFollowingResult { enum Type : int; }

UCLASS()
class PROJECTOPENWORLD_API UPalCommandExecutor_Mining : public UPalCommandExecutorBase
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	AActor* TargetResource{};
	UPROPERTY()
	APalAIController* OwnerController{};
	UPROPERTY()
	ABaseCreature* OwnerPal{};
	bool bStartedMining{};
	UPROPERTY()
	TObjectPtr<UInteractionComponent> InteractionComp{};
public:
	virtual void Initialize(UPalCommandComponent* CommandComp) override;
	virtual bool StartCommand(const FPalCommand& Command) override;
	virtual void Abort() override;

protected:
	UFUNCTION()
	void EndMining();

	UFUNCTION()
	void FinishMove(FAIRequestID RequestID, EPathFollowingResult::Type Result);
};
