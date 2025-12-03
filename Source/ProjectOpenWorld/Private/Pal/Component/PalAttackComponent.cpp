#include "Pal/Component/PalAttackComponent.h"

UPalAttackComponent::UPalAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPalAttackComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPalAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPalAttackComponent::SetAttackData(FPalAttackData sData)
{
	AttackData = sData;
}

void UPalAttackComponent::StartAttack()
{
	if (OnPalAttackStart.IsBound())
	{
		OnPalAttackStart.Broadcast();
	}
}
void  UPalAttackComponent::EndAttack()
{
	if (OnPalAttackEnd.IsBound())
	{
		OnPalAttackEnd.Broadcast();
	}
}

