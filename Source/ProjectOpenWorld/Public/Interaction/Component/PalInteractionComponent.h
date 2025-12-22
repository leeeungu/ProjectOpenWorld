#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interaction/Component/InteractionComponent.h"
#include "PalInteractionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UPalInteractionComponent : public UInteractionComponent
{
	GENERATED_BODY()
protected:

	UPalInteractionComponent();
protected:
	virtual void BeginPlay() override;

//public:	
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
