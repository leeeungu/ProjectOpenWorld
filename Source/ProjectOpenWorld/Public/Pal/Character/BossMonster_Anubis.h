#pragma once

#include "CoreMinimal.h"
#include "Pal/Character/BossMonster.h"
#include "BossMonster_Anubis.generated.h"

class UPalPatternComponent;
class UWidgetComponent;

UCLASS()
class PROJECTOPENWORLD_API ABossMonster_Anubis : public ABossMonster
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, Category = "BossPattern")
	TObjectPtr<UWidgetComponent> PatternWidget{};
protected:
	virtual void BeginPlay() override;

public:
	ABossMonster_Anubis();

	virtual void Tick(float DeltaTime) override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	UFUNCTION()
	virtual void OnCustomModeDamaged(AActor* Other, float Damaage);

	void StartPatternWidget(int nCount);
	void EndPatternWidget();
};
