#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Pal/Data/PalDamageType.h"
#include "AttackInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UAttackInterface : public UInterface
{
	GENERATED_BODY()
};

class AActor;
class UPalHitHandlerComponent;

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
	bool KnockBackAttack(const TScriptInterface< IAttackInterface>& Other);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	bool LaunchAttack(const TScriptInterface< IAttackInterface>& Other, FVector LaunchVelocity, bool bXYOverride, bool bZOverride);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	bool StunAttack(const TScriptInterface< IAttackInterface>& Other ,float StunTime);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	bool HitReaction(const TScriptInterface< IAttackInterface>& Other);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack")
	bool IsDead() const;

	virtual UPalHitHandlerComponent* GetHitHandlerComponent() const { return nullptr; }
};
