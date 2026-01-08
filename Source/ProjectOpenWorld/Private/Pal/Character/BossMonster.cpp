#include "Pal/Character/BossMonster.h"
#include "Pal/Component/PalPatternComponent.h"

ABossMonster::ABossMonster()
{
	PatternComponent = CreateDefaultSubobject<UPalPatternComponent>(TEXT("PatternComponent"));
}

void ABossMonster::SetStunned(float Duration)
{
	bStunned = true;
	StunDuration = Duration;
	CurStunTime = 0.0f;
}

void ABossMonster::ResetStunned()
{
	bStunned = false;
	StunDuration = 0.f;
}

void ABossMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bStunned)
	{
		CurStunTime += DeltaTime;
		if (CurStunTime >= StunDuration)
		{
			ResetStunned();
			CurStunTime = 0.f;
		}
	}
}
