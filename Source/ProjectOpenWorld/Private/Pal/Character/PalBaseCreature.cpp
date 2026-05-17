#include "Pal/Character/PalBaseCreature.h"
#include "Pal/Controller/PalAIlyController.h"
#include "Pal/Component/PalHitHandlerComponent.h"
#include "Pal/Component/PalMonsterCombatComponent.h"
#include "Pal/Component/PalAttackComponent.h"
#include "Pal/Component/PalJobComponent.h"
#include "GameBase/Component/StatComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

APalBaseCreature::APalBaseCreature() : Super{}
{
	AIControllerClass = APalAIlyController::StaticClass();
	
	AttackComponent = CreateDefaultSubobject<UPalAttackComponent>(TEXT("AttackComponent"));
	//InteractionComponent = CreateDefaultSubobject<UPalInteractionComponent>(TEXT("InteractionComponent"));

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking, 0);

	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("StatComponent"));
	HitHandlerComponent = CreateDefaultSubobject<UPalHitHandlerComponent>(TEXT("HitHandlerComponent"));
	JobComponent = CreateDefaultSubobject<UPalJobComponent>(TEXT("JobComponent"));

}

void APalBaseCreature::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

float APalBaseCreature::GetAttackValue_Implementation() const
{
	if (!StatComponent)
		return -1.0f;
	return StatComponent->GetCurrentStat(EStatusType::Attack);
}

bool APalBaseCreature::IsDead_Implementation() const
{
	if (!StatComponent)
		return true;
	return StatComponent->GetCurrentStat(EStatusType::HP) <= 0;
}

void APalBaseCreature::BeginPlay()
{
	Super::BeginPlay();
	if (HitHandlerComponent)
	{
		if (StatComponent)
		{
			HitHandlerComponent->OnDamageTaken.AddUniqueDynamic(StatComponent, &UStatComponent::ReceiveDamage);
		}
	}
	if (StatComponent && StatComponent->GetCurrentOnStatChanged(EStatusType::HP))
	{
		StatComponent->GetCurrentOnStatChanged(EStatusType::HP)->AddUniqueDynamic(this, &APalBaseCreature::HPChanged);
	}
}

void APalBaseCreature::HPChanged(double PreCurrentStat, double CurrentStat)
{
	UE_LOG(LogTemp, Warning, TEXT("HP Changed: %f -> %f"), PreCurrentStat, CurrentStat);
	if (CurrentStat <= 0)
	{
		//bDead = true;
		AttackComponent->StopAttack();
		if (GetMesh())
		{
			GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
			GetMesh()->SetSimulatePhysics(true);
		}
		GetMesh()->bPauseAnims = true;
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		FTimerHandle handle{};
		GetWorldTimerManager().SetTimer(handle, [this]() { Destroy(); }, 4.0f, false, 4.0f);
	}
}
