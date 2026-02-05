#include "Pal/Character/BossMonster.h"
#include "Pal/Component/PalPatternComponent.h"
#include "Components/WidgetComponent.h"
#include "Pal/Widget/PalHpWidget.h"
#include "Pal/Component/PalAttackComponent.h"
#include "Pal/Component/PalCommandComponent.h"
#include "Pal/Factory/PalCommandFunctionLibrary.h"
#include "Player/Character/BasePlayer.h"

void ABossMonster::BeginPlay()
{
	Super::BeginPlay();
	if (HpWidgetComponent)
	{
		HpWidgetClass = HpWidgetComponent->GetWidgetClass();
		if (HpWidgetClass)
		{
			HpWidget = CreateWidget<UUserWidget>(GetWorld(), HpWidgetClass);
			if(UPalHpWidget* HpWidgetCast = Cast<UPalHpWidget>(HpWidget))
			{
				HpWidgetCast->InitializeHPWidget(this);
			}
		}
		HpWidgetComponent->DestroyComponent();
	}
}

ABossMonster::ABossMonster() : Super()
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

void ABossMonster::OnDetectBeginEvent_Implementation(ABasePlayer* Player)
{
	if(HpWidget)
		HpWidget->AddToViewport();
	if (AttackComponent && !AttackComponent->IsSetTarget())
	{
		AttackComponent->SetAttackTarget(Player);
	}
	if (PalCommand->IsValidCommand() && PalCommand->GetCurrentCommandKind() != EPalCommandKind::Attack)
	{
		PalCommand->ResetCommandQue();
		IPalCommandInterface::Execute_ReceiveCommand(this, UPalCommandFunctionLibrary::CommandAttack(this, Player, ESubAttackType::Default));
	}
}

void ABossMonster::OnDetectEndEvent_Implementation(ABasePlayer* Player)
{
	if(HpWidget)
		HpWidget->RemoveFromParent();
}
