#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Pal/Data/PalCommandData.h"
#include "PalJabComponent.generated.h"



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTOPENWORLD_API UPalJabComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPalJabComponent();

protected:
	virtual void BeginPlay() override;

public:

};
