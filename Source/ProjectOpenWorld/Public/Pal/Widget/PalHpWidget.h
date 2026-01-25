#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PalHpWidget.generated.h"

class ABaseCharacter;

UCLASS(Abstract)
class PROJECTOPENWORLD_API UPalHpWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UFUNCTION()
	virtual void OnDamageEvent(AActor* Other, float Damage) PURE_VIRTUAL(UPalHpWidget::OnDamageEvent, );
public :
	virtual  void InitializeHPWidget(ABaseCharacter* OwnerCharacter)PURE_VIRTUAL (UPalHpWidget::Initialize, );
};
