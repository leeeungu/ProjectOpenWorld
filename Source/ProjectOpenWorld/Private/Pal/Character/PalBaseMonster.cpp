#include "Pal/Character/PalBaseMonster.h"
#include "Pal/Controller/PalMonsterController.h"
#include "Pal/Component/PalPatrolComponent.h"
#include "Pal/Component/PalHitHandlerComponent.h"
#include "Pal/Component/PalMonsterCombatComponent.h"
#include "Pal/Component/PalAttackComponent.h"
#include "GameBase/Component/StatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Pal/DataTable/PalMonsterData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Pal/FunctionLibrary/PalSpawnBlueprintFunctionLibrary.h"
#include "Pal/Interface/PalCaptor.h"
#include "Pal/Subsystem/PalCharacterDataSubsystem.h"

APalBaseMonster::APalBaseMonster() : Super()
{
	AIControllerClass = APalMonsterController::StaticClass();
	AttackComponent = CreateDefaultSubobject<UPalAttackComponent>(TEXT("AttackComponent"));
	HitHandlerComponent = CreateDefaultSubobject<UPalHitHandlerComponent>(TEXT("HitHandlerComponent"));
	MonsterCombatComponent = CreateDefaultSubobject<UPalMonsterCombatComponent>(TEXT("MonsterCombatComponent"));
	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("StatComponent"));
	MonsterName = TEXT("Monster");
	Level = 1;
	SetCanAffectNavigationGeneration(false);
	GetCapsuleComponent()->SetCanEverAffectNavigation(false);
	//GetCapsuleComponent()->bDynamicObstacle = false;
	GetMesh()->SetCanEverAffectNavigation(false);
	//GetMesh()->bEnableUpdateRateOptimizations = true;
	//GetMesh()->bDisplayDebugUpdateRateOptimizations = false;
}

void APalBaseMonster::InitializeLevel(int32 nLevel)
{
	Level = nLevel;
	const FPalMonsterData* Data{};
	if (UPalCharacterDataSubsystem::GetPalMonsterData(GetPalName(), Data))
	{
		TObjectPtr<UDataTable> MonsterData = Data->LevelDataTable;
		if (MonsterData)
		{
			TArray<FPalMonsterLevelData*> Rows;
			const FPalMonsterLevelData* Target{};
			MonsterData->GetAllRows<FPalMonsterLevelData>(TEXT(""), Rows);
			if (Rows.IsValidIndex(nLevel - 1))
				Target = Rows[nLevel - 1];
			if (Rows.IsValidIndex(0))
				Target = Rows[0];
			if (Target)
			{
				StatComponent->SetPalLevelData(*Target);
			}
		}
	}
}

void APalBaseMonster::OnMoveSpeedChanged(double PreCurrentStat, double CurrentStat)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = CurrentStat;
	}
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

float APalBaseMonster::GetAttackDistance() const
{
	if (!AttackComponent)
		return 100.0f;
	return AttackComponent->GetAttackDistance();
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
			HitHandlerComponent->OnDamageTaken.AddUniqueDynamic(MonsterCombatComponent, &UPalMonsterCombatComponent::OnReceiveDamage);
		}
		if (StatComponent)
		{
			HitHandlerComponent->OnDamageTaken.AddUniqueDynamic(StatComponent, &UStatComponent::OnReceiveDamage);
		}
	}
	if (StatComponent)
	{
		StatComponent->GetCurrentOnStatChanged(EStatusType::HP)->AddUniqueDynamic(this, &APalBaseMonster::OnHPChanged);
		StatComponent->GetCurrentOnStatChanged(EStatusType::MoveSpeed)->AddUniqueDynamic(this, &APalBaseMonster::OnMoveSpeedChanged);
	}
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
}

void APalBaseMonster::OnHPChanged(double PreCurrentStat, double CurrentStat)
{
	if (CurrentStat <= 0)
	{
		if(AttackComponent)
			AttackComponent->StopAttack();
		if (GetMesh())
		{
			GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
			GetMesh()->SetSimulatePhysics(true);
			if (HitHandlerComponent)
			{
				FVector LaunchForce = FVector(0.f, 0.f, 500.f);
				AActor* pOther = HitHandlerComponent->GetDamageInstigator();
				if (pOther)
				{
					GetMesh()->AddForce((GetActorLocation() - pOther->GetActorLocation()).GetSafeNormal() * 1000.f * GetMesh()->GetMass());
					const FPalMonsterData* Data{};
					if (pOther->Implements< UPalCaptor>() && UPalCharacterDataSubsystem::GetPalMonsterData(GetPalName(), Data))
					{
						IPalCaptor::Execute_TryCaptureCreature(pOther, Data->CreatureClass);
					}
				}
			}
		}
		UPalSpawnBlueprintFunctionLibrary::SpawnItemsForCharacter(this, GetPalName(), GetActorTransform(), this);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		FTimerHandle handle{};
		GetWorldTimerManager().SetTimer(handle, [this]() { Destroy(); }, 4.0f, false, 4.0f);
	}
}
