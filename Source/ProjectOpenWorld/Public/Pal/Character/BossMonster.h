#pragma once

#include "CoreMinimal.h"
#include "Creature/Character/BaseMonster.h"
#include "BossMonster.generated.h"

class UPalPatternComponent;
class UPalMonsterInteractionComponent;

UCLASS()
class PROJECTOPENWORLD_API ABossMonster : public ABaseMonster
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "BossPattern")
	TObjectPtr<UPalPatternComponent> PatternComponent{};

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "BossPattern")
	bool bStunned{};
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "BossPattern")
	float StunDuration{};

	float CurStunTime{};
	TSubclassOf<UUserWidget> HpWidgetClass{};
	UPROPERTY()
	TObjectPtr< UUserWidget> HpWidget{};


	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr <UPalMonsterInteractionComponent> MonsterInteractionComponent{};

protected:
	virtual void BeginPlay() override;

public:
	ABossMonster();

	UFUNCTION(BlueprintPure, Category = "BossPattern")
	FORCEINLINE UPalPatternComponent* GetPatternComponent() const { return PatternComponent; }

	UFUNCTION(BlueprintPure, Category = "BossAction")
	FORCEINLINE UPalMonsterInteractionComponent* GetMonsterInteractionComponent() const { return MonsterInteractionComponent; }


	void SetStunned(float Duration);
	void ResetStunned();

	UFUNCTION(BlueprintPure, Category = "BossState")
	bool IsStunned() const { return bStunned; }

	UFUNCTION(BlueprintPure, Category = "BossState")
	float GetStunDuration() const { return StunDuration; }


	virtual void Tick(float DeltaTime) override;


	// IPlayerDetectInterface
	virtual void OnDetectBeginEvent_Implementation(ABasePlayer* Player)override;
	virtual void OnDetectEndEvent_Implementation(ABasePlayer* Player)override;

};
