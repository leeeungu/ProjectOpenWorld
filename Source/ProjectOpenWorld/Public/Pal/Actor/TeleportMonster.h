#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameBase/Interface/MonsterInteractionInterface.h"
#include "TeleportMonster.generated.h"

UCLASS()
class PROJECTOPENWORLD_API ATeleportMonster : public AActor, public IMonsterInteractionInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TeleportMonster")
	uint8 InteractionID{};

public:	
	ATeleportMonster();

protected:
	virtual void BeginPlay() override;


public:	
	// IMonsterInteractionInterface
	virtual void OnInteractionEvent_Implementation(ACharacter* TargetMonster) override;
	virtual uint8 GetInteractionID_Implementation() const override { return InteractionID; }

//	virtual void Tick(float DeltaTime) override;

};
