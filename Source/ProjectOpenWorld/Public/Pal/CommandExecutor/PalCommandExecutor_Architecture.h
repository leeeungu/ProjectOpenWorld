#pragma once

#include "CoreMinimal.h"
#include "Building/Interface/ArchitectureInterface.h"
#include "Pal/CommandExecutor/PalCommandExecutorBase.h"
#include "PalCommandExecutor_Architecture.generated.h"

class ABaseBuilding;
class ABaseCreature;
class APalAIController;
struct FAIRequestID;
namespace EPathFollowingResult { enum Type : int; }


DECLARE_LOG_CATEGORY_EXTERN(ArchitectureCommand, Log, All);
UCLASS()
class PROJECTOPENWORLD_API UPalCommandExecutor_Architecture : public UPalCommandExecutorBase, public IArchitectureInterface
{
	GENERATED_BODY()
protected:
	ABaseBuilding* TargetBuilding{};
	APalAIController* OwnerController{};
	ABaseCreature* OwnerPal{};
	bool bActionStart = false;
public:
	virtual void Initialize(UPalCommandComponent* CommandComp) override;
	virtual bool StartCommand(const FPalCommand& Command) override;
	virtual void Abort() override;


	virtual float GetArchitectSpeed_Implementation() const override;
	virtual void StartArchitect_Implementation(ABaseBuilding* Building) override;
	virtual void StopArchitect_Implementation(ABaseBuilding* Building) override;
	virtual void EndArchitect_Implementation(ABaseBuilding* Building) override;
protected:
	UFUNCTION()
	void EndBuilding();

	UFUNCTION()
	void FinishMove(FAIRequestID RequestID, EPathFollowingResult::Type Result);
};
