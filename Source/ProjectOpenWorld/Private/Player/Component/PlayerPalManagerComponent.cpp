#include "Player/Component/PlayerPalManagerComponent.h"
#include "Creature/Character/BaseCreature.h"

UPlayerPalManagerComponent::UPlayerPalManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerPalManagerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPlayerPalManagerComponent::OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{
	Super::OnUpdateTransform(UpdateTransformFlags, Teleport);
	if (!SpawnedPal)
		return;

	if( FVector::DistSquared2D(SpawnedPal->GetActorLocation(), GetComponentLocation()) >= DespawnDistance* DespawnDistance)
	{
		return;
	}
	if (FVector::DistSquared2D(SpawnedPal->GetActorLocation(), GetComponentLocation()) >= FollowDistance * FollowDistance)
	{
		//SpawnedPal->
		return;
	}
}

void UPlayerPalManagerComponent::SetTargetPal(TSubclassOf<ABaseCreature> PalClass)
{
}

void UPlayerPalManagerComponent::SpawnPal()
{
}

void UPlayerPalManagerComponent::DespawnPal()
{
}

//void UPlayerPalManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//}

