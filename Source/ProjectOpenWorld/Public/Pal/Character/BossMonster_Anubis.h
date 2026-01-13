#pragma once

#include "CoreMinimal.h"
#include "Pal/Character/BossMonster.h"
#include "BossMonster_Anubis.generated.h"

class UPalPatternComponent;

UCLASS()
class PROJECTOPENWORLD_API ABossMonster_Anubis : public ABossMonster
{
	GENERATED_BODY()
protected:
protected:
	virtual void BeginPlay() override;

public:
	ABossMonster_Anubis();

	virtual void Tick(float DeltaTime) override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	UFUNCTION()
	virtual void OnCustomModeDamaged(AActor* Other, float Damaage);
};
