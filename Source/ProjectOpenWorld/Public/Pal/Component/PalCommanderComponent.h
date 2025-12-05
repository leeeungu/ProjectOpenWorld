#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PalCommanderComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPENWORLD_API UPalCommanderComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPalCommanderComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
