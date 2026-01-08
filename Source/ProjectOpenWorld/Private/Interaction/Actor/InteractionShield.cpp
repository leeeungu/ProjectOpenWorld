#include "Interaction/Actor/InteractionShield.h"

AInteractionShield::AInteractionShield()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AInteractionShield::BeginPlay()
{
	Super::BeginPlay();
}

void AInteractionShield::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

