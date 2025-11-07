#include "PalSystem/Actor/PalSphere.h"
#include "PalSystem/BlueprintFunction/PalSphereBFL.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

APalSphere::APalSphere()
{
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	OnActorHit.AddDynamic(this, &APalSphere::Hitted);
}

void APalSphere::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);
	ThrowSphere();
}

void APalSphere::Hitted(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor)
	{
		OtherActor;
	}

}

void APalSphere::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//CurTime += DeltaTime * 500;
	//float MaxTime = 300.0f;
	//float Min{};
	//if (Curve)
	//	Curve->GetTimeRange(Min, MaxTime);
	//if (CurTime >= MaxTime)
	//{
	//	CurTime = 0;
	//	//SetActorTickEnabled(false);
	//}
	//
	//SetActorLocation(GetActorLocation()+ StartLocation * DeltaTime);

}

void APalSphere::ThrowSphere()
{
	SetActorTickEnabled(true);
	UGameplayStatics::SuggestProjectileVelocity_CustomArc(GetWorld(), StartLocation, GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 400);
	 
		//GetActorLocation();

}

