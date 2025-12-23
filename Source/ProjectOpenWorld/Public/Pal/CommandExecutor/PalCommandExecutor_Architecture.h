#pragma once

#include "CoreMinimal.h"
#include "Pal/CommandExecutor/PalCommandExecutorBase.h"
#include "PalCommandExecutor_Architecture.generated.h"

class ABaseBuilding;
class ABaseCreature;
class APalAIController;
struct FAIRequestID;
namespace EPathFollowingResult { enum Type : int; }


DECLARE_LOG_CATEGORY_EXTERN(ArchitectureCommand, Log, All);
UCLASS()
class PROJECTOPENWORLD_API UPalCommandExecutor_Architecture : public UPalCommandExecutorBase
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	ABaseBuilding* TargetBuilding{};
	UPROPERTY()
	APalAIController* OwnerController{};
	UPROPERTY()
	ABaseCreature* OwnerPal{};
	bool bActionStart = false;
public:
	virtual void Initialize(UPalCommandComponent* CommandComp) override;
	virtual bool StartCommand(const FPalCommand& Command) override;
	virtual void Abort() override;


protected:
	UFUNCTION()
	void EndBuilding();

	UFUNCTION()
	void FinishMove(FAIRequestID RequestID, EPathFollowingResult::Type Result);
};
