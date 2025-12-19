#pragma once

#include "CoreMinimal.h"
#include "Pal/CommandExecutor/PalCommandExecutorBase.h"
#include "PalCommandExecutor_MoveActor.generated.h"

class ACharacter;
class APalAIController;
struct FAIRequestID;
namespace EPathFollowingResult { enum Type : int; }

UCLASS()
class PROJECTOPENWORLD_API UPalCommandExecutor_MoveActor : public UPalCommandExecutorBase
{
	GENERATED_BODY()
protected:
	ACharacter* OwnerPal{};
	APalAIController* OwnerController{};
public:

	virtual void Initialize(UPalCommandComponent* CommandComp) override;
	virtual bool StartCommand(const FPalCommand& Command) override;
	virtual void Abort() override;
protected:
	UFUNCTION()
	void FinishMove(FAIRequestID RequestID, EPathFollowingResult::Type Result);
};
