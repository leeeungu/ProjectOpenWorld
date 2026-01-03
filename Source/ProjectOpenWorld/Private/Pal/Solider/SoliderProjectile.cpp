#include "Pal/Solider/SoliderProjectile.h"
#include "NiagaraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"

ASoliderProjectile::ASoliderProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	SetActorTickEnabled(true);

	AttackCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("AttackCollision"));
	RootComponent = AttackCollision;
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(AttackCollision);
	MeshComp->SetRelativeRotation(FRotator(0, -90, 0));
	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComp"));
	NiagaraComp->SetupAttachment(AttackCollision);
}

void ASoliderProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void ASoliderProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UE_LOG(LogTemp, Log, TEXT("Move"));
	FVector NewLocation = GetActorLocation() + (GetActorForwardVector() * MoveSpeed * DeltaTime);
	FHitResult Result{};
	SetActorLocation(NewLocation, true, &Result, ETeleportType::None);
	if (Result.bBlockingHit)
	{
		Destroy();
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

