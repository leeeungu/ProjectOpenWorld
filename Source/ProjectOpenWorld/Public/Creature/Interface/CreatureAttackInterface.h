#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CreatureAttackInterface.generated.h"

class UCreatureAttackComponent;

UINTERFACE(MinimalAPI, BlueprintType)
class UCreatureAttackInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTOPENWORLD_API ICreatureAttackInterface
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CreatureAction")
	UCreatureAttackComponent* GetAttackComponent() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CreatureAction")
	float GetAttackDamage() const;
	//void ActionStart(ECreatureActionType ActionType, UObject* TargetObject);
	//UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CreatureAction")
	//void ActionEnd();
};