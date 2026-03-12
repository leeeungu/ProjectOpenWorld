#include "Creature/Character/BaseMonster.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Pal/Component/PalAttackComponent.h"
#include "Pal/Controller/PalMonsterController.h"
#include "Pal/Factory/PalCommandFunctionLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Pal/Component/PalCommandComponent.h"
#include "Pal/DataTable/PalMonsterData.h"
#include "Components/WidgetComponent.h"
#include "Pal/Widget/PalHpWidget_MonsterDefault.h"
#include "Pal/Controller/PalAIController.h"
#include "GameBase/Component/StatComponent.h"
#include "GameBase/Subsystem/SoundGameInstanceSubsystem.h"
#include "Pal/Subsystem/PalCharacterDataSubsystem.h"
#include "Item/System/ItemDataSubsystem.h"



ABaseMonster::ABaseMonster() :
	ABaseCharacter{}
{
	AIControllerClass = APalMonsterController::StaticClass();
	PalCommand = CreateDefaultSubobject<UPalCommandComponent>(TEXT("PalCommandComponent"));
	AttackComponent = CreateDefaultSubobject<UPalAttackComponent>(TEXT("AttackComponent"));
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
	HpWidgetComponent->SetVisibility(false);
	MonsterName = TEXT("Monster");
	Level = 1;
	HPStat->SetCurrentStat(100.f);
	HPStat->SetMaxStat(100.f);
	AttackStat->SetCurrentStat(10.f);
}

void ABaseMonster::BeginPlay()
{
	ABaseCharacter::BeginPlay();
	
}

bool ABaseMonster::ReceiveCommand_Implementation(FPalCommand Command)
{
	if (PalCommand)
	{
		return PalCommand->PushCommand(Command);
	}
	return false;
}

float ABaseMonster::GetAttackValue_Implementation() const
{
	return AttackStat->GetCurrentStat();
}

void ABaseMonster::SetAttackValue_Implementation(float NewValue)
{
	AttackStat->SetCurrentStat(NewValue);
}

void ABaseMonster::PossessedBy(AController* NewController)
{
	ABaseCharacter::PossessedBy(NewController);
	/*if (IGenericTeamAgentInterface* newTeam = Cast<IGenericTeamAgentInterface>(NewController))
	{
		newTeam->SetGenericTeamId(FGenericTeamId(2));
	}*/

}

void ABaseMonster::SpawnItem()
{
	TArray<FPalItemDropData> DropItemList =
		UPalCharacterDataSubsystem::GetDropItemListByCharacterID(GetMonsterName());
	//UE_LOG(LogTemp, Warning, TEXT("APalBaseCamp::Monster Dead Drop Item %s %d"), *Monster->GetMonsterName().ToString(), DropItemList.Num());
	for (const FPalItemDropData& ItemData : DropItemList)
	{
		int Rate = FMath::RandRange(0, 100);
		if (Rate > ItemData.Rate)
			continue;

		UE_LOG(LogTemp, Warning, TEXT("APalBaseCamp::Monster Dead Drop Item Spawn %s "), *ItemData.ItemId.ToString());
		int nCount = FMath::RandRange(ItemData.min, ItemData.Max);
		UItemDataSubsystem::SpawnPalStaticItemVisualActorByName(GetWorld(), ItemData.ItemId, GetActorTransform(), nCount);
	}
	Destroy();
}

void ABaseMonster::SetPalMonsterLevelData(int lv, const FPalMonsterLevelData& LevelData)
{
	Level = lv;
	HPStat->SetMaxStat(LevelData.MaxHP);
	HPStat->SetCurrentStat(LevelData.MaxHP);
	AttackStat->SetCurrentStat(LevelData.AttackPower);
	DefendStat->SetCurrentStat(LevelData.Armor);
	if (HpWidgetComponent)
	{
		UPalHpWidget_MonsterDefault* HpWidget = Cast<UPalHpWidget_MonsterDefault>(HpWidgetComponent->GetUserWidgetObject());
		if (HpWidget)
		{
			HpWidget->InitializeHPWidget(this);
		}
	}
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = LevelData.MoveSpeed;
	}
}

float ABaseMonster::GetCurrentHp() const
{
	return HPStat->GetCurrentStat();
}

float ABaseMonster::GetMaxHp() const
{
	return HPStat->GetMaxStat();
}

void ABaseMonster::RetAttackValue_Implementation()
{
	//UE_LOG(LogTemp, Log, TEXT("ABaseMonster :: RetAttackValue Not set"));
	////Attack = 10.0f;
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
	Damage = HPStat->AddCurrentStat(-Damage);
	USoundGameInstanceSubsystem::DamageEventBGMSound(this);
	if (PalCommand && PalCommand->GetCurrentCommandKind() != EPalCommandKind::Attack)
	{
		PalCommand->ResetCommandQue();
		IPalCommandInterface::Execute_ReceiveCommand(this, UPalCommandFunctionLibrary::CommandAttack(this, pOther, ESubAttackType::Default));
	}
	if (OnDamagedDelegate.IsBound())
	{
		OnDamagedDelegate.Broadcast(pOther, Damage);
	}
	if (HPStat->GetCurrentStat() <= 0.f)
	{
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
		GetWorldTimerManager().SetTimer(handle, [this]() {SpawnItem(); }, 4.0f, false, 4.0f);
	}
	return true;
}

bool ABaseMonster::IsDead_Implementation() const
{
	return HPStat->GetCurrentStat() <= 0.f;
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

