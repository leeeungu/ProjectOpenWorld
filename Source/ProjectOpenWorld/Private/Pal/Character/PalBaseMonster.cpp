#include "Pal/Character/PalBaseMonster.h"
#include "Pal/Controller/PalMonsterController.h"
#include "Pal/Component/PalPatrolComponent.h"
#include "Pal/Component/PalHitHandlerComponent.h"
#include "Pal/Component/PalMonsterCombatComponent.h"
#include "Pal/Component/PalAttackComponent.h"
#include "GameBase/Component/StatComponent.h"
#include "Components/CapsuleComponent.h"

APalBaseMonster::APalBaseMonster() : Super()
{
	AIControllerClass = APalMonsterController::StaticClass();
	AttackComponent = CreateDefaultSubobject<UPalAttackComponent>(TEXT("AttackComponent"));
	HitHandlerComponent = CreateDefaultSubobject<UPalHitHandlerComponent>(TEXT("HitHandlerComponent"));
	MonsterCombatComponent = CreateDefaultSubobject<UPalMonsterCombatComponent>(TEXT("MonsterCombatComponent"));
	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("StatComponent"));
	MonsterName = TEXT("Monster");
	Level = 1;
}

void APalBaseMonster::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

float APalBaseMonster::GetAttackValue_Implementation() const
{
	if (!StatComponent)
		return -1.0f;
	return StatComponent->GetCurrentStat(EStatusType::Attack);
}

bool APalBaseMonster::IsDead_Implementation() const
{
	if (!StatComponent)
		return true;
	return StatComponent->GetCurrentStat(EStatusType::HP) <= 0;
}

void APalBaseMonster::OnDetectBeginEvent_Implementation(ABasePlayer* Player)
{
}

void APalBaseMonster::OnDetectEndEvent_Implementation(ABasePlayer* Player)
{
}

void APalBaseMonster::BeginPlay()
{
	Super::BeginPlay();
	if (HitHandlerComponent)
	{
		if (MonsterCombatComponent)
		{
			HitHandlerComponent->OnDamageTaken.AddUniqueDynamic(MonsterCombatComponent, &UPalMonsterCombatComponent::ReceiveDamage);
		}
		if (StatComponent)
		{
			HitHandlerComponent->OnDamageTaken.AddUniqueDynamic(StatComponent, &UStatComponent::ReceiveDamage);
		}
	}
	if (StatComponent && StatComponent->GetCurrentOnStatChanged(EStatusType::HP))
	{
		StatComponent->GetCurrentOnStatChanged(EStatusType::HP)->AddUniqueDynamic(this, &APalBaseMonster::HPChanged);
	}
}

void APalBaseMonster::HPChanged(double PreCurrentStat, double CurrentStat)
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
			//FVector LaunchForce = FVector(0.f, 0.f, 500.f);
			//if (pOther)
			//{
			//	GetMesh()->AddForce((GetActorLocation() - pOther->GetActorLocation()).GetSafeNormal() * 1000.f * GetMesh()->GetMass());
			//}
		}
		GetMesh()->bPauseAnims = true;
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		FTimerHandle handle{};
		GetWorldTimerManager().SetTimer(handle, [this]() { Destroy(); }, 4.0f, false, 4.0f);
	}
}
