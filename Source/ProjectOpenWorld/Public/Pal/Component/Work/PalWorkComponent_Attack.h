#pragma once

#include "CoreMinimal.h"
#include "Pal/Component/Work/PalWorkComponent.h"
#include "PalWorkComponent_Attack.generated.h"

class UPalAttackComponent;
class UAttackInterface;
enum class ESubAttackType : uint8;

UCLASS()
class PROJECTOPENWORLD_API UPalWorkComponent_Attack : public UPalWorkComponent
{
	GENERATED_BODY()
private:
	UPROPERTY()
	TObjectPtr < UPalAttackComponent> AttackComponent{};
	TScriptInterface < UAttackInterface> AttackTarget{};
	TWeakObjectPtr<AActor> TargetCharacter{};
	ESubAttackType CurrentSubAttackType{};
public:
	UPalWorkComponent_Attack();
protected:
	virtual void BeginPlay() override;
public:
	virtual bool CanWork() override;
	virtual void WorkStart(const FPalCommand& Command) override;
	virtual void WorkEvent(const FPalCommand& Command) override;
	virtual void WorkEnd(const FPalCommand& Command) override;
	virtual void WorkCancel() override;
};
