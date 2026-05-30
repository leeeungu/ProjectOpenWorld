#include "Pal/Component/PalHitHandlerComponent.h"
#include "Components/ShapeComponent.h"	

void UPalHitHandlerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPalHitHandlerComponent::TakeDamage(const FPalDamagePayload& Payload)
{
	// damage 계산?
	OnDamageTaken.Broadcast(Payload);
}