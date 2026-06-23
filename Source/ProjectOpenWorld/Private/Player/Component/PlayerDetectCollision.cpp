#include "Player/Component/PlayerDetectCollision.h"
#include "Player/Interface/PlayerDetectInterface.h"
#include "Player/Character/BasePlayer.h"


UPlayerDetectCollision::UPlayerDetectCollision() : Super()
{
	PrimaryComponentTick.bCanEverTick = false;
	SphereRadius = 3000.f;
	SetGenerateOverlapEvents(true);
	SetCollisionProfileName(TEXT("OverlapAll"));
}

void UPlayerDetectCollision::OnDetectBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//DrawDebugSphere(GetWorld(), SweepResult.ImpactPoint, 10.0F, 32, FColor::Green, false, 2.0f);
	if (OtherActor && OtherActor->GetClass()->ImplementsInterface(UPlayerDetectInterface::StaticClass()) && OwnerPlayer)
	{
		IPlayerDetectInterface::Execute_OnDetectBeginEvent(OtherActor, OwnerPlayer);
		OnDetectChanged.Broadcast(OtherActor, true);
	}
}

void UPlayerDetectCollision::OnDetectEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->GetClass()->ImplementsInterface(UPlayerDetectInterface::StaticClass()) && OwnerPlayer)
	{
		IPlayerDetectInterface::Execute_OnDetectEndEvent(OtherActor, OwnerPlayer);
		OnDetectChanged.Broadcast(OtherActor, false);
	}
}

void UPlayerDetectCollision::BeginPlay()
{
	Super::BeginPlay();
	OwnerPlayer = Cast<ABasePlayer>(GetOwner());
	OnComponentBeginOverlap.AddUniqueDynamic(this, &UPlayerDetectCollision::OnDetectBeginOverlap);
	OnComponentEndOverlap.AddUniqueDynamic(this, &UPlayerDetectCollision::OnDetectEndOverlap);
	for (const FOverlapInfo& OtherOverlap : OverlappingComponents)
	{
		UPrimitiveComponent* OtherComp = OtherOverlap.OverlapInfo.Component.Get();
		AActor* const OtherActor = OtherComp->GetOwner();
		if (OtherActor && OtherActor->GetClass()->ImplementsInterface(UPlayerDetectInterface::StaticClass()) && OwnerPlayer)
		{
			OnDetectChanged.Broadcast(OtherActor, true);
		}
	}
}
