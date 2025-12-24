#pragma once

#include "CoreMinimal.h"
#include "Pal/CommandExecutor/PalCommandExecutorBase.h"
#include "PalCommandExecutor_Transport.generated.h"


class ABaseCreature;
class APalAIController;
struct FAIRequestID;
class UInteractionComponent;
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
	UPROPERTY()
	TObjectPtr < APalAIController> OwnerController{};
	UPROPERTY()
	TObjectPtr < ABaseCreature> OwnerPal{};
	TransportState eTransportState = TransportState::None;

public:
	virtual void Initialize(UPalCommandComponent* CommandComp) override;
	virtual bool StartCommand(const FPalCommand& Command) override;
	virtual void Abort() override;
	virtual void WorkCommand() override;
	virtual bool CheckCommandValid() override;

protected:
	UFUNCTION()
	void EndTransport();

};
