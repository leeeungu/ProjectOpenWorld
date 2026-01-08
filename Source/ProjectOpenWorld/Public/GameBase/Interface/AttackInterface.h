#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AttackInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UAttackInterface : public UInterface
{
	GENERATED_BODY()
};

class AActor;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamageedDelegate, AActor*, Other, float , Damaage);

class IAttackInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	float GetAttackValue() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void  SetAttackValue(float NewValue);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	void  RetAttackValue();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	bool DamagedCharacter(const TScriptInterface< IAttackInterface>& Other);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	bool IsDead() const;
};
