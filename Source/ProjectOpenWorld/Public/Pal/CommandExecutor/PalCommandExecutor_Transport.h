#pragma once

#include "CoreMinimal.h"
#include "Pal/CommandExecutor/PalCommandExecutorBase.h"
#include "PalCommandExecutor_Transport.generated.h"


class ABaseCreature;
class APalAIController;
struct FAIRequestID;
namespace EPathFollowingResult { enum Type : int; }

UCLASS()
class PROJECTOPENWORLD_API UPalCommandExecutor_Transport : public UPalCommandExecutorBase
{
	GENERATED_BODY()
	enum class TransportState
	{
		None,
		Go,
		Back
	};
protected:
	APalAIController* OwnerController{};
	ABaseCreature* OwnerPal{};
	TransportState eTransportState = TransportState::None;


public:
	virtual void Initialize(UPalCommandComponent* CommandComp) override;
	virtual void StartCommand(const FPalCommand& Command) override;
	virtual void Abort() override;

protected:
	UFUNCTION()
	void EndTransport();

	UFUNCTION()
	void FinishMove(FAIRequestID RequestID, EPathFollowingResult::Type Result);
};
