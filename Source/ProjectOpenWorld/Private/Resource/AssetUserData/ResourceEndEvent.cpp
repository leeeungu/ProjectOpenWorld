#include "Resource/AssetUserData/ResourceEndEvent.h"
#include "GameBase/Subsystem/SoundGameInstanceSubsystem.h"


void UResourceEndEvent::ResourceEndEvent(UMeshComponent* OwnerComponent, FTransform MeshTransform)
{
	USoundGameInstanceSubsystem::PlayEffectSound(ResourceEndSound, MeshTransform.GetLocation());
	//if (ResourceEndSound)
	//{
	//	UGameplayStatics::PlaySoundAtLocation(OwnerComponent, ResourceEndSound, MeshTransform.GetLocation());
	//}
}
