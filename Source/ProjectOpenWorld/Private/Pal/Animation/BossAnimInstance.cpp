#include "Pal/Animation/BossAnimInstance.h"
#include "Pal/Character/BossMonster.h"

void UBossAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	OwnerBoss = Cast<ABossMonster>(TryGetPawnOwner());
}

void UBossAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!OwnerBoss)
	{
		OwnerBoss = Cast<ABossMonster>(TryGetPawnOwner());
	}
	if (OwnerBoss)
	{
		bIsStunned = OwnerBoss->IsStunned();
		StunDuration = OwnerBoss->GetStunDuration();
	}
}
