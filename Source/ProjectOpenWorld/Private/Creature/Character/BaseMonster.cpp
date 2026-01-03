#include "Creature/Character/BaseMonster.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Pal/Component/PalAttackComponent.h"
#include "Pal/Controller/PalMonsterController.h"
#include "Pal/Component/PalMonsterCommandComponent.h"
#include "Pal/Factory/PalCommandFunctionLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GenericTeamAgentInterface.h"

ABaseMonster::ABaseMonster() :
	ABaseCharacter{}
{
	AIControllerClass = APalMonsterController::StaticClass();
	CommandComponent = CreateDefaultSubobject<UPalMonsterCommandComponent>(TEXT("PalCommand"));
	AttackComponent = CreateDefaultSubobject<UPalAttackComponent>(TEXT("AttackComponent"));
	Hp = 100;
	Attack = 10.0f;
}

bool ABaseMonster::ReceiveCommand_Implementation(FPalCommand Command)
{
	if (CommandComponent)
	{
		return CommandComponent->PushCommand(Command);
	}
	return false;
}

float ABaseMonster::GetAttackValue_Implementation() const
{
	return Attack;
}

void ABaseMonster::SetAttackValue_Implementation(float NewValue)
{
	Attack = NewValue;
}

void ABaseMonster::PossessedBy(AController* NewController)
{
	ABaseCharacter::PossessedBy(NewController);
	if (IGenericTeamAgentInterface* newTeam = Cast<IGenericTeamAgentInterface>(NewController))
	{
		newTeam->SetGenericTeamId(FGenericTeamId(1));
	}
}

void ABaseMonster::RetAttackValue_Implementation()
{
	Attack = 1.0f;
}

bool ABaseMonster::DamagedCharacter_Implementation(const TScriptInterface<IAttackInterface>& Other)
{
	if (!Other || !Other.GetObject())
		return false;
	APawn* pOther = Cast < APawn>(Other.GetObject());
	if (!pOther || FGenericTeamId::GetAttitude(GetController(), pOther->GetController()) == ETeamAttitude::Friendly)
	{
		return false;
	}
	float Damage = IAttackInterface::Execute_GetAttackValue(Other.GetObject());
	if (Hp < Damage)
		Damage = Hp;
	Hp -= Damage;
	if (CommandComponent->IsValidCommand() && CommandComponent->GetCurrentCommandKind() != EPalCommandKind::Attack)
	{
		CommandComponent->ResetCommandQue();
	}
	if (AttackComponent && pOther && !CommandComponent->IsValidCommand() && CommandComponent->GetCurrentCommandKind() != EPalCommandKind::Attack)
	{
		//UE_LOG(LogTemp, Log, TEXT("ABaseMonster :: Attack"), Hp);
		CommandComponent->PushCommand(UPalCommandFunctionLibrary::CommandAttack(this, pOther, ESubAttackType::Default));
	}
	if (OnDamagedDelegate.IsBound())
	{
		OnDamagedDelegate.Broadcast(pOther);
	}
	//UE_LOG(LogTemp, Log, TEXT("HP : %f"), Hp);
	if (Hp <= 0.f)
	{
		Hp = 0.0f;
		if (GetMesh())
		{
			GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
			GetMesh()->SetSimulatePhysics(true);
			if (pOther)
			{
				GetMesh()->AddForce((GetActorLocation() - pOther->GetActorLocation()).GetSafeNormal() * 1000.f * GetMesh()->GetMass());
			}
		}
		GetMesh()->bPauseAnims = true;
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		FTimerHandle handle{};
		bDead = true;
		GetWorldTimerManager().SetTimer(handle, [this]() {Destroy(); }, 4.0f,false, 4.0f);
	}
	return true;
}

bool ABaseMonster::IsDead_Implementation() const
{
	return Hp <= 0.f;
}

EPalCommandKind ABaseMonster::GetCommandKind_Implementation()
{
	return EPalCommandKind::Attack;
}

uint8 ABaseMonster::GetSubCommandType_Implementation()
{
	return static_cast<uint8>(ESubAttackType::Default);
}

FPalCommand ABaseMonster::GetCommand_Implementation()
{
	return UPalCommandFunctionLibrary::CommandAttack(this, this, ESubAttackType::Default);
}

bool ABaseMonster::IsCommandFinished_Implementation()
{
	return IsPendingKillPending();
}

