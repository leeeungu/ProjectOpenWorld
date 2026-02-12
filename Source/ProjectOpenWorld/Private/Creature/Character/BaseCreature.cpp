#include "Creature/Character/BaseCreature.h"
#include "Navigation/PathFollowingComponent.h"
#include "Pal/Component/PalCommandComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Pal/Factory/PalCommandFunctionLibrary.h"
#include "Pal/Controller/PalAIController.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GenericTeamAgentInterface.h"
#include "Interaction/Component/PalInteractionComponent.h"
#include "GameBase/Component/StatComponent.h"
#include "UObject/ObjectSaveContext.h"

void ABaseCreature::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->SetUpdateNavAgentWithOwnersCollisions(true);
}

ABaseCreature::ABaseCreature() : Super{}
{
	AIControllerClass = APalAIController::StaticClass();
	ArchitectureMeshComponent = CreateDefaultSubobject< UStaticMeshComponent>(TEXT("ArchitectureMesh"));
	if (ArchitectureMeshComponent)
	{
		ArchitectureMeshComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("SWeapon_R"));
		ArchitectureMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
		ArchitectureMeshComponent->SetVisibility(false);
	}
	PalCommand = CreateDefaultSubobject<UPalCommandComponent>(TEXT("PalCommandComponent"));
	AttackComponent  = CreateDefaultSubobject<UPalAttackComponent>(TEXT("AttackComponent"));
	InteractionComponent = CreateDefaultSubobject<UPalInteractionComponent>(TEXT("InteractionComponent"));

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking, 0);
	HPStat->SetCurrentStat(300.0f);
	HPStat->SetMaxStat(300.0f);
	AttackStat->SetCurrentStat(10.0f);

	
}

bool ABaseCreature::ReceiveCommand_Implementation(FPalCommand Command)
{
	if (PalCommand)
	{
		return PalCommand->PushCommand(Command);
	}
	return false;
}

bool ABaseCreature::CommandPause_Implementation(bool bPause)
{
	if (PalCommand)
	{
		PalCommand->FinishCommand();
		return true;
	}
	return false;
}

UPalCommandComponent* ABaseCreature::GetCommandComponent() const
{ 
	return PalCommand;
}

float ABaseCreature::GetDefaultSpeed()
{
	return 300.0f;
}

void ABaseCreature::SetActionStarted(bool bValue)
{
	bActionStarted = bValue;
}

void ABaseCreature::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (IGenericTeamAgentInterface* newTeam = Cast<IGenericTeamAgentInterface>(NewController))
	{
		newTeam->SetGenericTeamId(FGenericTeamId(0));
	}
}

bool ABaseCreature::DamagedCharacter_Implementation(const TScriptInterface<IAttackInterface>& Other)
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
	//if (CommandComponent->IsValidCommand() && CommandComponent->GetCurrentCommandKind() != EPalCommandKind::Attack)
	//{
	//	CommandComponent->ResetCommandQue();
	//}
	//if (AttackComponent && pOther && !CommandComponent->IsValidCommand() && CommandComponent->GetCurrentCommandKind() != EPalCommandKind::Attack)
	//{
	//	//UE_LOG(LogTemp, Log, TEXT("ABaseMonster :: Attack"), Hp);
	//	CommandComponent->PushCommand(UPalCommandFunctionLibrary::CommandAttack(this, pOther, ESubAttackType::Default));
	//}
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
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		FTimerHandle handle{};
		bDead = true;
		GetWorldTimerManager().SetTimer(handle, [this]() {Destroy(); }, 4.0f, false, 4.0f);
	}
	return true;
}

bool ABaseCreature::IsDead_Implementation() const
{
	return bDead;
}

void ABaseCreature::OnDetectBeginEvent_Implementation(ABasePlayer* Player)
{

}

void ABaseCreature::OnDetectEndEvent_Implementation(ABasePlayer* Player)
{
}


float ABaseCreature::GetArchitectSpeed_Implementation() const
{
	return 1.f;
}

void ABaseCreature::StartArchitect_Implementation(ABaseBuilding* Building)
{
	SetActionStarted(true);
	if (GetArchitectureMeshComponent())
		GetArchitectureMeshComponent()->SetVisibility(true);
}

void ABaseCreature::StopArchitect_Implementation(ABaseBuilding* Building)
{
	SetActionStarted(false);
	if (GetArchitectureMeshComponent())
		GetArchitectureMeshComponent()->SetVisibility(false);
	GetCommandComponent()->FinishCommand();
}

void ABaseCreature::EndArchitect_Implementation(ABaseBuilding* Building)
{
	SetActionStarted(false);
	if (GetArchitectureMeshComponent())
		GetArchitectureMeshComponent()->SetVisibility(false);
	GetCommandComponent()->FinishCommand();
}


float ABaseCreature::GetResourceSpeed_Implementation() const
{
	return 1.0f;
}

void ABaseCreature::StartResource_Implementation(AResourceActor* ResourceActor)
{
	SetActionStarted(true);
}

void ABaseCreature::StopResource_Implementation(AResourceActor* ResourceActor)
{
	SetActionStarted(false);
	GetCommandComponent()->FinishCommand();
}

void ABaseCreature::EndResource_Implementation(AResourceActor* ResourceActor)
{
	SetActionStarted(false);
	GetCommandComponent()->FinishCommand();
}

float ABaseCreature::GetAttackValue_Implementation() const
{
	return AttackStat->GetCurrentStat();
}

void ABaseCreature::SetAttackValue_Implementation(float NewValue)
{
	AttackStat->SetCurrentStat(NewValue);
}

void ABaseCreature::RetAttackValue_Implementation()
{
	//Attack = 1.0f;
}
