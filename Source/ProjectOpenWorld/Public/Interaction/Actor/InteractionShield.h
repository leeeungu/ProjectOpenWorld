#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractionShield.generated.h"

UCLASS()
class PROJECTOPENWORLD_API AInteractionShield : public AActor
{
	GENERATED_BODY()
	
public:	
	AInteractionShield();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
