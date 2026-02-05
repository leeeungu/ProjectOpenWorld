#include "Pal/Actor/AnubisSolverProjectile.h"
#include "Player/Character/BasePlayer.h"
#include "Interaction/Component/InteractionComponent.h"
#include "Components/SphereComponent.h"
#include "Pal/Character/BossMonster.h"
#include "Pal/Component/PalPatternComponent.h"
#include "Pal/Object/PatternObj_Anubis.h"

AAnubisSolverProjectile::AAnubisSolverProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	SetActorTickEnabled(false);
	

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(Collision);
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	
	PatternType = EAnubisPatternType::Anubis_Pattern_01;
	bProjectileActive = false;
}

// Called when the game starts or when spawned
void AAnubisSolverProjectile::BeginPlay()
{
	Super::BeginPlay();
	Mesh->SetVisibility(false);

	Collision->OnComponentBeginOverlap.AddUniqueDynamic(this, &AAnubisSolverProjectile::OnAnubisSolverBeginOverlap);
	Collision->OnComponentEndOverlap.AddUniqueDynamic(this, &AAnubisSolverProjectile::OnAnubisSolverEndOverlap);
}

void AAnubisSolverProjectile::OnAnubisSolverBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{

		ABossMonster* AnubisBoss = Cast<ABossMonster>(OtherActor);
		if (AnubisBoss && bProjectileActive)
		{
			uint8 Index = static_cast<uint8>(PatternType);
			SetActorTickEnabled(false);
			bProjectileActive = false;
			if (AnubisBoss->GetPatternComponent()->IsPatternActive(Index))
			{
				AnubisBoss->GetPatternComponent()->UpdatePatternCondition(Index);
			}
		}
		else if(ABasePlayer* Player = Cast<ABasePlayer>(OtherActor))
		{
			TargetPlayer = Player;
			UInteractionComponent* InteractionComp = TargetPlayer->GetInteractionComponent();
			if (!InteractionComp)
				return;
			InteractionComp->SetInteractionTarget(this);
	
		}
		else if (bProjectileActive)
		{
			SetActorTickEnabled(false);
			bProjectileActive = false;
		}
	}
}

void AAnubisSolverProjectile::OnAnubisSolverEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (TargetPlayer == OtherActor && !OtherActor)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Anubis Solver Projectile End Overlap with Player"));
		UInteractionComponent* InteractionComp = TargetPlayer->GetInteractionComponent();
		if (!InteractionComp)
			return;
		InteractionComp->SetInteractionTarget(nullptr);
		TargetPlayer = nullptr;
	}
}

void AAnubisSolverProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//MoveDirection = GetActorForwardVector();
	FVector NewLocation = GetActorLocation() + (MoveDirection * MoveSpeed * DeltaTime);
	SetActorLocation(NewLocation);
	FRotator NewRotation = { 0,RotateSpeed * DeltaTime,0 };
	AddActorWorldRotation(NewRotation);
	FVector Location = GetActorLocation();
}

void AAnubisSolverProjectile::OnInteractionStart_Implementation(ACharacter* pOther)
{
	APlayerController* PC = Cast<APlayerController>(pOther->GetController());
	if ( !TargetPlayer || !TargetPlayer->IsTopDownMode())
		return;
	if (PC)
	{
		SetActorTickEnabled(true);
		float MouseX = 0.f, MouseY = 0.f;
		// 마우스 좌표 얻기
		if (PC->GetMousePosition(MouseX, MouseY))
		{
			FVector WorldOrigin, WorldDir;
			if (PC->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldOrigin, WorldDir))
			{
				const FVector TraceStart = WorldOrigin;
				const FVector TraceEnd = WorldOrigin + WorldDir * 100000.0f;

				FHitResult Hit;
				FCollisionQueryParams Params{};
				Params.AddIgnoredActor(this);
				bProjectileActive = true;

				GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params);
				if (Hit.GetActor())
				{
					FVector TargetLocation = Hit.Location;
					FVector Direction = (TargetLocation - GetActorLocation()).GetSafeNormal2D();
					MoveDirection = Direction;
				}
			}
		}
	}
	if (TargetPlayer)
	{
		UInteractionComponent* InteractionComp = TargetPlayer->GetInteractionComponent();
		if (!InteractionComp)
			return;
		InteractionComp->OnInteractionCompleted();
	}
}

////////////////////////////// IMonsterInteractionInterface ///////////////////////////////////////////
void AAnubisSolverProjectile::OnInteractionEvent_Implementation(ACharacter* TargetMonster)
{
}

void AAnubisSolverProjectile::OnInteractionStartEvent_Implementation(ACharacter* TargetMonster)
{
	Mesh->SetVisibility(true);
}

void AAnubisSolverProjectile::OnInteractionEndEvent_Implementation(ACharacter* TargetMonster)
{
	Mesh->SetVisibility(false);
	//Destroy();
}

