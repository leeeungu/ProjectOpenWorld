#pragma once

#include "CoreMinimal.h"
#include "Creature/Component/CreatureActionComponent.h"
#include "CreatureAction_Transport.generated.h"

UCLASS()
class PROJECTOPENWORLD_API UCreatureAction_Transport : public UCreatureActionComponent
{
	GENERATED_BODY()
protected:
	TSoftObjectPtr<AActor> TransportActor{};
	TSoftObjectPtr<AActor> BoxActor{};
public:
	UCreatureAction_Transport();
	virtual bool ActionStart_Implementation(AActor* SendActor, AActor* TargetActor = nullptr) override;
	virtual bool ActionEnd_Implementation() override;
};
