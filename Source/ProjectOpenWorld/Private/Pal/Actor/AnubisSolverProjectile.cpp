#include "Pal/Actor/AnubisSolverProjectile.h"
#include "Player/Character/BasePlayer.h"
#include "Interaction/Component/InteractionComponent.h"

AAnubisSolverProjectile::AAnubisSolverProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	SetActorTickEnabled(false);

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->OnComponentBeginOverlap.AddDynamic(this, &AAnubisSolverProjectile::OnPlayerOverlap);
}

// Called when the game starts or when spawned
void AAnubisSolverProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AAnubisSolverProjectile::OnPlayerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		ABasePlayer* Player = Cast<ABasePlayer>(OtherActor);
		if (!Player)
		{
			SetActorTickEnabled(false);
			return;
		}
		UInteractionComponent* InteractionComp = Player->GetInteractionComponent();
		if (!InteractionComp)
			return;
		InteractionComp->SetInteractionTarget(this);
	}
}

// Called every frame
void AAnubisSolverProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MoveDirection = GetActorForwardVector();
	FVector NewLocation = GetActorLocation() + (MoveDirection * MoveSpeed * DeltaTime);
	SetActorLocation(NewLocation);
	FRotator NewRotation = { 0,RotateSpeed * DeltaTime,0 };
	AddActorWorldRotation(NewRotation);
}

void AAnubisSolverProjectile::OnInteractionStart_Implementation(ACharacter* pOther)
{
	SetActorTickEnabled(true);

}

void AAnubisSolverProjectile::OnInteractionEnd_Implementation(ACharacter* pOther)
{
	if (ABasePlayer* Player = Cast<ABasePlayer>(pOther))
	{
		UInteractionComponent* InteractionComp = Player->GetInteractionComponent();
		if (!InteractionComp)
			return;
		InteractionComp->SetInteractionTarget(nullptr);
	}
}

