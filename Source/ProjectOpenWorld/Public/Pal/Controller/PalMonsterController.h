#pragma once

#include "CoreMinimal.h"
#include "Pal/Controller/PalAIController.h"
#include "PalMonsterController.generated.h"

class UPalAIStateComponent;
enum class EPalAIMonsterState : uint8;

UCLASS()
class PROJECTOPENWORLD_API APalMonsterController : public APalAIController
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, Category = "PalAIState")
	TObjectPtr<UPalAIStateComponent> AIStateComponent{};

	//UFUNCTION()
	//void RecieveDamage(const struct FPalDamagePayload& DamagePayload);

	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;
public:
	APalMonsterController();
	virtual FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId(2); }
	UFUNCTION(BlueprintCallable, Category = "PalAIBlackBoard")
	void SetTargetActor(AActor* Target);
	UFUNCTION(BlueprintPure, Category = "PalAIState")
	UPalAIStateComponent* GetAIStateComponent() const { return AIStateComponent; }
};
