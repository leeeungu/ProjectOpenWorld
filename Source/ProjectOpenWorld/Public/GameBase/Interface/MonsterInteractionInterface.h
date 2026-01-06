#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MonsterInteractionInterface.generated.h"

UINTERFACE(MinimalAPI)
class UMonsterInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTOPENWORLD_API IMonsterInteractionInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Monster Interaction")
	void OnInteractionEvent(ACharacter* TargetMonster);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Monster Interaction")
	uint8 GetInteractionID() const;

	// IMonsterInteractionInterface
	//virtual void OnInteractionEvent_Implementation(ACharacter* TargetMonster) override;
	//virtual uint8 GetInteractionID_Implementation() const override;
};
