#include "Player/Component/PlayerDetectCollision.h"
#include "Player/Interface/PlayerDetectInterface.h"
#include "Player/Character/BasePlayer.h"


UPlayerDetectCollision::UPlayerDetectCollision() : Super()
{
	PrimaryComponentTick.bCanEverTick = false;
	SphereRadius = 2000.f;
	SetGenerateOverlapEvents(true);
	SetCollisionProfileName(TEXT("OverlapAll"));
}

void UPlayerDetectCollision::OnDetectBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->GetClass()->ImplementsInterface(UPlayerDetectInterface::StaticClass()) && OwnerPlayer)
	{
		IPlayerDetectInterface::Execute_OnDetectBeginEvent(OtherActor, OwnerPlayer);
	}
}

void UPlayerDetectCollision::OnDetectEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->GetClass()->ImplementsInterface(UPlayerDetectInterface::StaticClass()) && OwnerPlayer)
	{
		IPlayerDetectInterface::Execute_OnDetectEndEvent(OtherActor, OwnerPlayer);
	}
}

void UPlayerDetectCollision::BeginPlay()
{
	Super::BeginPlay();
	OwnerPlayer = Cast<ABasePlayer>(GetOwner());
	OnComponentBeginOverlap.AddDynamic(this, &UPlayerDetectCollision::OnDetectBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UPlayerDetectCollision::OnDetectEndOverlap);
}
