#include "Pal/Character/BossMonster.h"
#include "Pal/Component/PalPatternComponent.h"
#include "Components/WidgetComponent.h"
#include "Pal/Widget/PalHpWidget.h"
#include "Pal/Component/PalAttackComponent.h"
#include "Pal/Component/PalCommandComponent.h"
#include "Pal/Factory/PalCommandFunctionLibrary.h"
#include "Player/Character/BasePlayer.h"
#include "Pal/Component/PalMonsterInteractionComponent.h"
#include "Pal/Data/PalCommandData.h"

void ABossMonster::BeginPlay()
{
	Super::BeginPlay();
}

ABossMonster::ABossMonster() : Super()
{
	PrimaryActorTick.bCanEverTick = false;
	PatternComponent = CreateDefaultSubobject<UPalPatternComponent>(TEXT("PatternComponent"));
	MonsterInteractionComponent = CreateDefaultSubobject<UPalMonsterInteractionComponent>(TEXT("MonsterInteractionComponent"));
}

void ABossMonster::SetStunned(float Duration)
{
	bStunned = true;
	GetWorldTimerManager().SetTimer(StunTimerHandle, this, &ABossMonster::ResetStunned, Duration, false);
}

void ABossMonster::ResetStunned()
{
	bStunned = false;
	GetWorldTimerManager().ClearTimer(StunTimerHandle);
}

void ABossMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}