#include "GameBase/Actor/ProjectileAttack.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/ShapeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Pal/Data/PalCollisionFactory.h"
#include "GameBase/Object/AttackObject.h"
#include "Pal/Data/PalDamageType.h"

AProjectileAttack::AProjectileAttack()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	Movement->bRotationFollowsVelocity = true;
	Movement->ProjectileGravityScale = 0.f;
	Movement->MaxSpeed = 0.f; 
}

void AProjectileAttack::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ApplyCollisionProfile();
}

void AProjectileAttack::BeginPlay()
{
	Super::BeginPlay();

	Collision = GetCollision();
	if (!ensureMsgf(Collision, TEXT("AProjectileAttack: GetCollision이 콜리전을 반환하지 않음 - BP에서 지정 필요")))
	{
		return;
	}

	ApplyCollisionProfile();
	if(Collision)
		Collision->OnComponentBeginOverlap.AddDynamic(this, &AProjectileAttack::OnProjectileOverlap);
}

void AProjectileAttack::Launch(const FProjectileLaunchParams& Params)
{
	Movement->ProjectileGravityScale = Params.GravityScale;
	Movement->Velocity = GetActorForwardVector() * Params.ProjectileSpeed;

	if (Params.LifeSpan > 0.f)
	{
		SetLifeSpan(Params.LifeSpan);
	}
}

void AProjectileAttack::ApplyCollisionProfile()
{
	UPrimitiveComponent* Col = Collision ? Collision.Get() : GetCollision();
	if (!Col)
	{
		return;
	}
	Col->SetCollisionProfileName(PalDamage::GetHitCollisionProfileName());
}

UPrimitiveComponent* AProjectileAttack::GetCollision_Implementation() const
{
	return FindComponentByClass<UPrimitiveComponent>();
}

void AProjectileAttack::ResolveHit_Implementation(const FHitResult& Hit)
{
	Destroy();
}

void AProjectileAttack::OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetInstigator() || OtherActor->GetClass() == GetClass())
	{
		return;
	}
	FAttackEventContext Contxt{};
	Contxt.Owner = GetInstigator();
	Contxt.Hit = &SweepResult;
	Contxt.Causer = this;
	for (UAttackObject* AttackObject : AttackEventObject)
	{
		if (AttackObject)
		{
			AttackObject->ExecuteAttackEvent(Contxt);
		}
	}
	OnProjectileHit(SweepResult);
	//ResolveHit(SweepResult);
}

void AProjectileAttack::OnHandleHit(UPrimitiveComponent* /*HitComp*/, AActor* OtherActor, UPrimitiveComponent* /*OtherComp*/, FVector /*NormalImpulse*/, const FHitResult& Hit)
{
	//FAttackEventContext Contxt{};
	//Contxt.Owner = GetInstigator();
	//Contxt.Hit = &SweepResult;
	//Contxt.Causer = this;
	//for (UAttackObject* AttackObject : AttackEventObject)
	//{
	//	if (AttackObject)
	//	{
	//		AttackObject->ExecuteAttackEvent(Contxt);
	//	}
	//}
	//
	//OnProjectileHit(Hit);
	//ResolveHit(Hit);
}