#include "Resource/AssetUserData/HarvestableAssetUserData.h"
#include "Kismet/GameplayStatics.h"

void UHarvestableAssetUserData::PlayHarvestHitSound(UMeshComponent* OwnerComponent, FTransform MeshTransform) const
{
	if (HitSound && OwnerComponent)
	{
		UGameplayStatics::PlaySoundAtLocation(OwnerComponent->GetWorld(), HitSound, MeshTransform.GetLocation(), HitVolumeMultiplier, HitPitchMultiplier);
	}
}

void UHarvestableAssetUserData::PlayHarvestDestroySound(UMeshComponent* OwnerComponent, FTransform MeshTransform) const
{
	if (DestroySound && OwnerComponent)
	{
		UGameplayStatics::PlaySoundAtLocation(OwnerComponent->GetWorld(), DestroySound, MeshTransform.GetLocation(), DestroyVolumeMultiplier, DestroyPitchMultiplier);
	}
}
