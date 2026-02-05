#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "Interaction/InteractionInterface.h"
#include "InteractionWidgetComponent.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UInteractionWidgetComponent : public UWidgetComponent
	, public IInteractionInterface
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
public:
	// IInteractionInterface
	virtual void OnBeginDetected_Implementation(ACharacter* pOther) override;
	virtual void OnEndDetected_Implementation(ACharacter* pOther) override;
	virtual void OnInteractionStart_Implementation(ACharacter* pOther) override;
	virtual void OnInteraction_Implementation(ACharacter* pOther) override;
	virtual void OnInteractionEnd_Implementation(ACharacter* pOther) override;
	virtual void OnInteractionCanceled_Implementation(ACharacter* pOther) override;

	
};
