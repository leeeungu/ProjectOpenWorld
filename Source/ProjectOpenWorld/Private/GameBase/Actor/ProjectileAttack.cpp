#include "GameBase/Actor/ProjectileAttack.h"
#include "Components/CapsuleComponent.h"
#include "GameBase/Interface/AttackInterface.h"

AProjectileAttack::AProjectileAttack() :Super()
{
	PrimaryActorTick.bCanEverTick = true;
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Collision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision"));
	Collision->SetupAttachment(Root);
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	CurrentDistanceTraveled = 0.0f;
}

void AProjectileAttack::InitializeProjectile(TScriptInterface<IAttackInterface> OwnerAttacker, const FVector& Direction, float Speed, float LifeTime, float DistanceTraveled)
{
	if ((LifeTime == 0 && DistanceTraveled == 0) || !OwnerAttacker)
	{
		Destroy();
		return;
	}
	ProjectileOwnerAttacker = OwnerAttacker;
	ProjectileDirection = Direction.GetSafeNormal();
	ProjectileSpeed = Speed;
	ProjectileLifeTime = LifeTime;
	SetLifeSpan(ProjectileLifeTime);
	ProjectileDistanceTraveled = DistanceTraveled;
}

void AProjectileAttack::BeginPlay()
{
	Super::BeginPlay();
	ProjectileOwnerAttacker = GetInstigator();
	if (Collision)
	{
		Collision->OnComponentBeginOverlap.AddUniqueDynamic(this, &AProjectileAttack::OnProjectileOverlap);
	}
}

void AProjectileAttack::OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!ProjectileOwnerAttacker || OtherActor == ProjectileOwnerAttacker.GetObject() || OtherActor == GetInstigator() || OtherActor->GetClass() == GetClass())
	{
		return;
	}

	if (OtherActor && OtherActor->Implements<UAttackInterface>())
	{
		IAttackInterface::Execute_DamagedCharacter(OtherActor, ProjectileOwnerAttacker);
	}
}

void AProjectileAttack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float DeltaDistance = ProjectileSpeed * DeltaTime;
	if(CurrentDistanceTraveled + DeltaDistance >= ProjectileDistanceTraveled)
	{
		DeltaDistance = ProjectileDistanceTraveled - CurrentDistanceTraveled;
	}
	CurrentDistanceTraveled += DeltaDistance;
	FVector DeltaLocation = ProjectileDirection * DeltaDistance;
	AddActorWorldOffset(DeltaLocation, true);
	if (CurrentDistanceTraveled >= ProjectileDistanceTraveled)
	{
		Destroy();
	}
}