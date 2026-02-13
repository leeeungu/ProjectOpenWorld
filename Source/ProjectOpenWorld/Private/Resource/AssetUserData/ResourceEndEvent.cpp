#include "Resource/AssetUserData/ResourceEndEvent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


void UResourceEndEvent::ResourceEndEvent(UMeshComponent* OwnerComponent, FTransform MeshTransform)
{
	if (ResourceEndSound)
	{
		UGameplayStatics::PlaySoundAtLocation(OwnerComponent, ResourceEndSound, MeshTransform.GetLocation());
	}
}
