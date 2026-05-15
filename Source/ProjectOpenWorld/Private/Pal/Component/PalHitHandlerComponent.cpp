#include "Pal/Component/PalHitHandlerComponent.h"
#include "Components/ShapeComponent.h"	

void UPalHitHandlerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPalHitHandlerComponent::TakeDamage(const FPalDamagePayload& Payload)
{
	OnDamageTaken.Broadcast(Payload);
}