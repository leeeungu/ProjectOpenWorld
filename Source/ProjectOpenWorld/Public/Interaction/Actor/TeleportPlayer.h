#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/InteractionInterface.h"
#include "TeleportPlayer.generated.h"

class ATargetPoint;
class USphereComponent;

USTRUCT(BlueprintType)
struct FTeleportData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<ATargetPoint>  TeleportPointA{};
	UPROPERTY(EditAnywhere)
	TObjectPtr<USphereComponent> InteractionA{};

};

UCLASS()
class PROJECTOPENWORLD_API ATeleportPlayer : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
protected:
public:	
	ATeleportPlayer();

protected:
	virtual void BeginPlay() override;

public:	

};
