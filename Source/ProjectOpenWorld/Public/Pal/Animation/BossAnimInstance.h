#pragma once

#include "CoreMinimal.h"
#include "Pal/Animation/MonsterAnimInstance.h"
#include "BossAnimInstance.generated.h"

class ABossMonster;

UCLASS()
class PROJECTOPENWORLD_API UBossAnimInstance : public UMonsterAnimInstance
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, Category = "PalAnim")
	TObjectPtr<ABossMonster> OwnerBoss{};
	UPROPERTY(BlueprintReadOnly, Category = "PalAnim")
	bool bIsStunned{};
	UPROPERTY(BlueprintReadOnly, Category = "PalAnim")
	float StunDuration{};

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
