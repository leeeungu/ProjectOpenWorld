#include "Creature/Character/BaseMonster.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Pal/Component/PalAttackComponent.h"
#include "Pal/Controller/PalMonsterController.h"
#include "Pal/Component/PalMonsterCommandComponent.h"
#include "Pal/Factory/PalCommandFunctionLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Pal/Component/PalMonsterInteractionComponent.h"
#include "Pal/DataTable/PalMonsterData.h"
#include "Components/WidgetComponent.h"
#include "Pal/Widget/PalHpWidget_MonsterDefault.h"
#include "Pal/Controller/PalAIController.h"



ABaseMonster::ABaseMonster() :
	ABaseCharacter{}
{
	AIControllerClass = APalMonsterController::StaticClass();
	CommandComponent = CreateDefaultSubobject<UPalMonsterCommandComponent>(TEXT("PalCommand"));
	AttackComponent = CreateDefaultSubobject<UPalAttackComponent>(TEXT("AttackComponent"));
	MonsterInteractionComponent = CreateDefaultSubobject<UPalMonsterInteractionComponent>(TEXT("MonsterInteractionComponent"));
	HpWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HpWidgetComponent"));
	//Script/UMGEditor.WidgetBlueprint'/Game/Pal/Widget/WBP_PalMonsterHP.WBP_PalMonsterHP'
	static ConstructorHelpers::FClassFinder<UPalHpWidget_MonsterDefault> HpWidgetClass(TEXT("/Game/Pal/Widget/WBP_PalMonsterHP"));
	if (HpWidgetClass.Succeeded())
	{
		HpWidgetComponent->SetWidgetClass(HpWidgetClass.Class);
		UPalHpWidget_MonsterDefault* HpWidget = Cast<UPalHpWidget_MonsterDefault>(HpWidgetComponent->GetUserWidgetObject());
		if (HpWidget)
		{
			HpWidget->InitializeHPWidget(this);
		}
	}
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking, 0);
	HpWidgetComponent->SetTickMode(ETickMode::Enabled);
	HpWidgetComponent->SetupAttachment(GetRootComponent());
	HpWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	MonsterName = TEXT("Monster");
	Level = 1;
	Hp = 100;
	Attack = 10.0f;
}

void ABaseMonster::BeginPlay()
{
	ABaseCharacter::BeginPlay();
	if (HpWidgetComponent)
	{
		UPalHpWidget_MonsterDefault* HpWidget = Cast<UPalHpWidget_MonsterDefault>(HpWidgetComponent->GetUserWidgetObject());
		if (HpWidget)
		{
			HpWidget->InitializeHPWidget(this);
		}
	}
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
	/*if (IGenericTeamAgentInterface* newTeam = Cast<IGenericTeamAgentInterface>(NewController))
	{
		newTeam->SetGenericTeamId(FGenericTeamId(2));
	}*/

}

void ABaseMonster::SetPalMonsterLevelData(int lv, const FPalMonsterLevelData& LevelData)
{
	Level = lv;
	Hp = LevelData.MaxHP;
	Attack = LevelData.AttackPower;
	Defend = LevelData.Armor;
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = LevelData.MoveSpeed;
	}
}

void ABaseMonster::RetAttackValue_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("ABaseMonster :: RetAttackValue Not set"));
	//Attack = 10.0f;
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

		UE_LOG(LogTemp, Log, TEXT("ABaseMonster :: Attack Target Set"));
		APalAIController* AIController = Cast<APalAIController>(GetController());
		if (AIController)
		{
			AIController->SetBBTargetActor(pOther);
		}
	if (AttackComponent && !AttackComponent->IsSetTarget())
	{
		AttackComponent->SetAttackTarget(pOther);
		//pOther && !CommandComponent->IsValidCommand() && CommandComponent->GetCurrentCommandKind() != EPalCommandKind::Attack)
	/*	AttackComponent->SetAttackTarget(pOther);
		AttackComponent->SetAttackData(ESubAttackType::Default);
		AttackComponent->StartAttack();*/
		//UE_LOG(LogTemp, Log, TEXT("ABaseMonster :: Attack"), Hp);
	}
	if (CommandComponent->IsValidCommand() && CommandComponent->GetCurrentCommandKind() != EPalCommandKind::Attack)
	{
		CommandComponent->ResetCommandQue();
		IPalCommandInterface::Execute_ReceiveCommand(this, UPalCommandFunctionLibrary::CommandAttack(this, pOther, ESubAttackType::Default));
	}
	UE_LOG(LogTemp, Log, TEXT("ABaseMonster :: DamagedCharacter Hp : %f"), Hp);
	if (OnDamagedDelegate.IsBound())
	{
		OnDamagedDelegate.Broadcast(pOther, Damage);
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

void ABaseMonster::OnDetectBeginEvent_Implementation(ABasePlayer* Player)
{
	if (HpWidgetComponent)
	{
		HpWidgetComponent->SetVisibility(true);
	}
}

void ABaseMonster::OnDetectEndEvent_Implementation(ABasePlayer* Player)
{
	if (HpWidgetComponent)
	{
		HpWidgetComponent->SetVisibility(false);
	}
}

