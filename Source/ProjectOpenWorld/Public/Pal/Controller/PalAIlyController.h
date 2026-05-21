#pragma once

#include "CoreMinimal.h"
#include "Pal/Controller/PalAIController.h"
#include "PalAIlyController.generated.h"

struct FPalWorkCommand;

UCLASS()
class PROJECTOPENWORLD_API APalAIlyController : public APalAIController
{
	GENERATED_BODY()

public:
	APalAIlyController();

	virtual FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId(1); }

protected:
	virtual void OnPossess(APawn* InPawn) override;
	UFUNCTION()
	void OnJobAssigned(const FPalWorkCommand& Job);
	UFUNCTION()
	void OnJobFinished(const FPalWorkCommand& Job, bool bSuccess);
	UFUNCTION()
	void OnJobLocationChange(FVector NewLocation);
	UFUNCTION()
	void OnJobTargetChange(AActor* NewActor);
};
