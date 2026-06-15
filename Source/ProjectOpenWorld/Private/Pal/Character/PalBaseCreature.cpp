#include "Pal/Character/PalBaseCreature.h"
#include "Pal/Controller/PalAIlyController.h"
#include "Pal/Component/PalHitHandlerComponent.h"
#include "Pal/Component/PalMonsterCombatComponent.h"
#include "Pal/Component/PalAttackComponent.h"
#include "Pal/Component/PalJobComponent.h"
#include "GameBase/Component/StatComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Pal/Animation/PalCreatureAnimInstance.h"
#include "Pal/Data/PalSocketProfile.h"

APalBaseCreature::APalBaseCreature() : Super{}
{
	AIControllerClass = APalAIlyController::StaticClass();
	
	AttackComponent = CreateDefaultSubobject<UPalAttackComponent>(TEXT("AttackComponent"));
	//InteractionComponent = CreateDefaultSubobject<UPalInteractionComponent>(TEXT("InteractionComponent"));

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking, 0);

	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("StatComponent"));
	HitHandlerComponent = CreateDefaultSubobject<UPalHitHandlerComponent>(TEXT("HitHandlerComponent"));
	JobComponent = CreateDefaultSubobject<UPalJobComponent>(TEXT("JobComponent"));

	JobToolComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PalToolMesh"));
	JobToolComponent->SetupAttachment(GetMesh());
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

float APalBaseCreature::GetAttackDistance() const
{
	return AttackComponent->GetAttackDistance();
}

bool APalBaseCreature::IsDead_Implementation() const
{
	if (!StatComponent)
		return true;
	return StatComponent->GetCurrentStat(EStatusType::HP) <= 0;
}

void APalBaseCreature::StartWorking()
{
	if (JobComponent)
	{
		JobComponent->StartWorking();
	}
}

void APalBaseCreature::StopWorking()
{
	if (JobComponent)
	{
		JobComponent->StopWorking();
	}
}

void APalBaseCreature::EndWorking(bool bSuccess)
{
	if (JobComponent)
	{
		JobComponent->EndWorking(bSuccess);
	}
}

float APalBaseCreature::GetWorkSpeed(EPalJobType JobType)
{
	if(!StatComponent)
		return 0.0f;
	return StatComponent->GetCurrentStat(PalStatus::GetJobWorkSpeedStatus(JobType));
}



void APalBaseCreature::SetTransportWorkMoveSpeed(float MaxMoveSpeed)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = MaxMoveSpeed;
	}
}

void APalBaseCreature::OnStartTransport()
{
	if (!JobComponent || JobComponent->GetCurrentJobType() != EPalJobType::Transport || !GetCharacterMovement() || !StatComponent)
		return;
	double Speed = GetCharacterMovement()->MaxWalkSpeed * StatComponent->GetCurrentStat(EStatusType::TransportSpeed);
	SetTransportWorkMoveSpeed(Speed);
	AActor* Actor = JobComponent->GetTransportActor();
	if (Actor)
	{
		UPrimitiveComponent* OtherRoot = Cast< UPrimitiveComponent>(Actor->GetRootComponent());
		if (OtherRoot)
		{
			OtherRoot->SetSimulatePhysics(false);
		}
		Actor->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, PalSocketProfile::GetPalTransportSocekt());
	}
}

void APalBaseCreature::OnEndTransport()
{
	if (!JobComponent || JobComponent->GetCurrentJobType() != EPalJobType::Transport || !GetCharacterMovement() || !StatComponent)
		return;
	double Speed = GetCharacterMovement()->MaxWalkSpeed / StatComponent->GetCurrentStat(EStatusType::TransportSpeed);
	SetTransportWorkMoveSpeed(Speed);
	AActor* Actor = JobComponent->GetTransportActor();
	if (Actor)
	{
		UPrimitiveComponent* OtherRoot = Cast< UPrimitiveComponent>(Actor->GetRootComponent());
		if (OtherRoot)
		{
			OtherRoot->SetSimulatePhysics(true);
		}
		Actor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
}

void APalBaseCreature::OnWorkMeshChanged(UStaticMesh* NewMesh, FName SocketName, FTransform SocketTransform)
{
	if (!JobToolComponent)
		return;
	JobToolComponent->SetStaticMesh(NewMesh);
	JobToolComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, SocketName);
	JobToolComponent->SetRelativeTransform(SocketTransform);
}

void APalBaseCreature::BeginPlay()
{
	Super::BeginPlay();
	if (HitHandlerComponent)
	{
		if (StatComponent)
		{
			HitHandlerComponent->OnDamageTaken.AddUniqueDynamic(StatComponent, &UStatComponent::OnReceiveDamage);
		}
	}
	if (StatComponent && StatComponent->GetCurrentOnStatChanged(EStatusType::HP))
	{
		StatComponent->GetCurrentOnStatChanged(EStatusType::HP)->AddUniqueDynamic(this, &APalBaseCreature::OnHPChanged);
	}
	if (JobComponent && GetMesh())
	{
		if (UPalCreatureAnimInstance* Anim = Cast<UPalCreatureAnimInstance>(GetMesh()->GetAnimInstance()))
		{
			//JobComponent->OnJobAssigned.AddUniqueDynamic(Anim, &UPalCreatureAnimInstance::OnChangeWork);
			Anim->OnToolMeshChanged.AddUniqueDynamic(this, &APalBaseCreature::OnWorkMeshChanged);
			JobComponent->OnWorkStart.AddUObject(Anim, &UPalCreatureAnimInstance::OnStartWork);
			JobComponent->OnWorkEnd.AddUObject(Anim, &UPalCreatureAnimInstance::OnEndWork);
			JobComponent->OnJobAssigned.AddUniqueDynamic(Anim, &UPalCreatureAnimInstance::OnChangeWorkCommand);

			JobComponent->OnWorkStart.AddUObject(this, &APalBaseCreature::OnStartTransport);
			JobComponent->OnWorkEnd.AddUObject(this, &APalBaseCreature::OnEndTransport);
		}
	}
}

void APalBaseCreature::OnHPChanged(double PreCurrentStat, double CurrentStat)
{
	UE_LOG(LogTemp, Warning, TEXT("%s Hitted %f"), *GetName(), CurrentStat);
	if (CurrentStat <= 0)
	{
		AttackComponent->StopAttack();
		if (GetMesh())
		{
			GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
			GetMesh()->SetSimulatePhysics(true);
			FVector LaunchForce = FVector(0.f, 0.f, 500.f);
			AActor* pOther = HitHandlerComponent->GetDamageInstigator();
			if (pOther)
			{
				GetMesh()->AddForce((GetActorLocation() - pOther->GetActorLocation()).GetSafeNormal() * 1000.f * GetMesh()->GetMass());
			}
		}
		GetMesh()->bPauseAnims = true;
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		FTimerHandle handle{};
		GetWorldTimerManager().SetTimer(handle, [this]() { Destroy(); }, 4.0f, false, 4.0f);
	}
}
