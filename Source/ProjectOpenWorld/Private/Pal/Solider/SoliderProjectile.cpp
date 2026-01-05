#include "Pal/Solider/SoliderProjectile.h"
#include "NiagaraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Player/Character/BasePlayer.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interaction/Component/InteractionComponent.h"

ASoliderProjectile::ASoliderProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	SetActorTickEnabled(true);

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	AttackCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("AttackCollision"));
	AttackCollision ->SetupAttachment(RootComponent);
	AttackCollision->SetCollisionProfileName(TEXT("NoCollision"));
	AttackCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	AttackCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	AttackCollision->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	AttackCollision->SetRelativeRotation(FRotator(-90, 0, 0));
	AttackCollision->SetRelativeLocation(FVector(0, 0, 0));
	AttackCollision->SetCapsuleSize(10, 120);
	AttackCollision->OnComponentBeginOverlap.AddDynamic(this, &ASoliderProjectile::ProjectileOverlap);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionProfileName(TEXT("NoCollision"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetRelativeRotation(FRotator(0, -90, 0));
	PreViewMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PreViewMeshComp"));
	PreViewMeshComp->SetupAttachment(RootComponent);
	
	{
		//Script/Engine.StaticMesh'/Game/Pal/Model/Weapon/Spear/SM_SoldierBee_spear.SM_SoldierBee_spear'
		ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectilMesh(TEXT("/Game/Pal/Model/Weapon/Spear/SM_SoldierBee_spear.SM_SoldierBee_spear"));
		if (ProjectilMesh.Succeeded())
		{
			MeshComp->SetStaticMesh(ProjectilMesh.Object);
		}
	}
	{
		//Script/Engine.StaticMesh'/Game/Pal/Model/Weapon/Spear/SM_PreviewSoldierBee_spear.SM_PreviewSoldierBee_spear'
		ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectilMesh(TEXT("/Game/Pal/Model/Weapon/Spear/SM_PreviewSoldierBee_spear.SM_PreviewSoldierBee_spear"));
		if (ProjectilMesh.Succeeded())
		{
			PreViewMeshComp->SetStaticMesh(ProjectilMesh.Object);

		}
	}
	//Script/Engine.Material'/Game/Pal/Model/Weapon/Spear/Material/M_ProjectilePreview.M_ProjectilePreview'

	PreViewMeshComp->SetVisibility(false);
	PreViewMeshComp->SetHiddenInGame(true);
	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComp"));
	NiagaraComp->SetupAttachment(RootComponent);
}

void ASoliderProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void ASoliderProjectile::ProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)
		return;
	TScriptInterface<IAttackInterface> AttackInterface = TScriptInterface<IAttackInterface>(OtherActor);
	if (AttackInterface && OtherActor != GetInstigator())
	{
		if(AttackInterface->Execute_DamagedCharacter(OtherActor, GetInstigator()))
			Destroy();
	}
}

void ASoliderProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector NewLocation = GetActorLocation() + (GetActorForwardVector() * MoveSpeed * DeltaTime);
	SetActorLocation(NewLocation, false);
	FHitResult HitResult{};
	if (UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		GetActorLocation(),
		GetActorLocation() + (GetActorForwardVector() * 2000.0f),
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Pawn),
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::None,
		HitResult,
		true
	))
	{
		ABasePlayer* Player = Cast<ABasePlayer>(HitResult.GetActor());
		if (Player)
		{
			if (Player->GetInteractionComponent())
			{
				Player->GetInteractionComponent()->SetInteractionTarget(this);
			}
			NewLocation = Player->GetActorLocation();
			NewLocation.Z = GetActorLocation().Z;
			FRotator NewRot = FRotator(0,Player->GetControlRotation().Yaw,0);
			PreViewMeshComp->SetWorldRotation(NewRot + FRotator(0,-90,0));
			PreViewMeshComp->SetWorldLocation(NewLocation);
			PreViewMeshComp->AddWorldOffset((NewRot.Quaternion() * FVector(1, 0, 0)) * (AttackCollision->GetScaledCapsuleHalfHeight() * 1.1f));
			PreViewMeshComp->SetVisibility(true);
			PreViewMeshComp->SetHiddenInGame(false);
		}
		else
		{
			PreViewMeshComp->SetVisibility(false);
			PreViewMeshComp->SetHiddenInGame(true);
		}

	}
	else
	{
		PreViewMeshComp->SetVisibility(false);
		PreViewMeshComp->SetHiddenInGame(true);
	}
}
void ASoliderProjectile::ChangeDirection(const FVector& NewDirection)
{
	double angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(GetActorForwardVector().GetSafeNormal2D(), NewDirection.GetSafeNormal2D())));
	float Dir = FVector::DotProduct(GetActorRightVector().GetSafeNormal2D(), NewDirection.GetSafeNormal2D());
	if (Dir < 0)
		angle = -angle;
	
	AddActorWorldRotation(FRotator(0,angle, 0));
}


void ASoliderProjectile::OnInteractionStart_Implementation(ACharacter* pOther)
{
	ABasePlayer* Player = Cast<ABasePlayer>(pOther);
	if (Player && GetSquaredDistanceTo(pOther) < InteractionDistance * InteractionDistance)
	{
		if (Player->GetInteractionComponent())
		{
			if (Player->GetInteractionComponent()->GetTargetActor() != this)
				return;
			SetInstigator(Player);
			Player->GetInteractionComponent()->ResetInteractionTarget(this);
		}
		FVector NewLocation = Player->GetActorLocation();
		NewLocation.Z = GetActorLocation().Z;
		FRotator NewRot = FRotator(0, Player->GetControlRotation().Yaw, 0);
		SetActorRotation(NewRot);
		SetActorLocation(NewLocation);
		AddActorWorldOffset((NewRot.Quaternion() * FVector(1,0,0)) * (AttackCollision->GetScaledCapsuleHalfHeight() * 1.1f));
		PreViewMeshComp->SetVisibility(false);
		PreViewMeshComp->SetHiddenInGame(true);
		
	}
}

