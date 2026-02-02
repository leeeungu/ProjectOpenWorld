#include "Pal/Solider/SpawnSolider.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"

ASpawnSolider::ASpawnSolider() : AActor()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	SplineComp = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComp"));
	SplineComp->SetupAttachment(RootComponent);
}

void ASpawnSolider::BeginPlay()
{
	Super::BeginPlay();
	SpawnLocation = SplineComp->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
	SpawnDirection = SplineComp->GetDirectionAtSplinePoint(0, ESplineCoordinateSpace::World);
	if (SoliderClass)
	{
		SpawnOffset = FVector::ZeroVector;
		// double 타입을 float 타입으로 변환하여 전달
		float CapsuleRadius = static_cast<float>(SpawnOffset.X);
		float CapsuleHalfHeight = static_cast<float>(SpawnOffset.Y);
		SoliderClass->GetDefaultObject<ACharacter>()->GetCapsuleComponent()->GetScaledCapsuleSize(CapsuleRadius, CapsuleHalfHeight);
		SpawnOffset.Y = SpawnOffset.X = CapsuleRadius * 2.0f +10.f;
		SpawnOffset.Z = CapsuleHalfHeight * 2.0f;
	}
	FVector endLocation = SplineComp->GetLocationAtSplinePoint(SplineComp->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);
	SpawnCount = FMath::CeilToInt(FVector::Distance(SpawnLocation, endLocation) / SpawnOffset.X);
	arSolider.Reserve(SpawnCount);
	for (uint8 i = 0; i < SpawnCount; ++i)
	{
		arSolider.Push(SpawnSolider());
		arSolider[i]->SetActorTickEnabled(false);
		arSolider[i]->SetActorHiddenInGame(true);
		arSolider[i]->SetActorEnableCollision(false);
	}
}

ACharacter* ASpawnSolider::SpawnSolider()
{
	if (SoliderClass)
	{
		SpawnLocation += SpawnDirection * SpawnOffset.X;
		SpawnLocation.Z += SpawnOffset.Z;
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		ACharacter* SpawnedSolider = GetWorld()->SpawnActor<ACharacter>(SoliderClass, SpawnLocation, SpawnRotator, SpawnParams);
		return SpawnedSolider;
	}
	return nullptr;
}

void ASpawnSolider::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASpawnSolider::OnInteractionEvent_Implementation(ACharacter* TargetMonster)
{
	UE_LOG(LogTemp, Warning, TEXT("ASpawnSolider::OnInteractionEvent_Implementation Spawn Solider Count : %d"), SpawnCount);
	for (uint8 i = 0; i < SpawnCount; ++i)
	{
		arSolider[i]->SetActorTickEnabled(true);
		arSolider[i]->SetActorHiddenInGame(false);
		arSolider[i]->SetActorEnableCollision(true);
	}
}

void ASpawnSolider::OnInteractionStartEvent_Implementation(ACharacter* TargetMonster)
{
	IMonsterInteractionInterface::Execute_OnInteractionEvent(this, TargetMonster);
}

void ASpawnSolider::OnInteractionEndEvent_Implementation(ACharacter* TargetMonster)
{
	for (ACharacter* Soilder : arSolider)
	{
		Soilder->Destroy();
	}
	arSolider.Empty(SpawnCount);
	SpawnLocation = SplineComp->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
}

