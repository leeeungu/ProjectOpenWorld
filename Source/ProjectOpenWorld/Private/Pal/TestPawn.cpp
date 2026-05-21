#include "Pal/TestPawn.h"
#include "Components/SphereComponent.h"

ATestPawn::ATestPawn() :Super{}
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetupAttachment(GetRootComponent());
}

void ATestPawn::BeginPlay()
{
	Super::BeginPlay();
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ATestPawn::OnBeginOverlap);
	Collision->OnComponentEndOverlap.AddDynamic(this, &ATestPawn::OnEndOverlap);
	for (const FOverlapInfo& OtherOverlap : Collision->GetOverlapInfos())
	{
		UPrimitiveComponent* OtherComp = OtherOverlap.OverlapInfo.Component.Get();
		AActor* const OtherActor = OtherComp->GetOwner();
		Collision->OnComponentBeginOverlap.Broadcast(Collision, OtherActor, OtherComp, OtherOverlap.GetBodyIndex(), OtherOverlap.bFromSweep, OtherOverlap.OverlapInfo);
	}
}

void ATestPawn::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("OnBeginOverlap :: %s"), *OtherActor->GetName());

}

void ATestPawn::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("OnEndOverlap :: %s"), *OtherActor->GetName());
}