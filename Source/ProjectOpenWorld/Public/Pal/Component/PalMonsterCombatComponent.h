#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PalMonsterCombatComponent.generated.h"

class UPalAIStateComponent;
class APalMonsterController;
struct FPalDamagePayload;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UPalMonsterCombatComponent : public UActorComponent
{
	GENERATED_BODY()
private:
	UPROPERTY()
	TObjectPtr< UPalAIStateComponent>  AIStateComponent{};
	UPROPERTY()
	TObjectPtr <APalMonsterController> MonsterController{};
protected:
	virtual void BeginPlay() override;
public:
	UFUNCTION()
	void OnReceiveDamage(const FPalDamagePayload& DamagePayload);
	void ResetTarget();

	void SetCombatTarget(AActor* Target);
};
