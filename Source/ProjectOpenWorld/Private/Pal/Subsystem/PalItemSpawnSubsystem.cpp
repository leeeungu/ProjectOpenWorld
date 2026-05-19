#include "Pal/Subsystem/PalItemSpawnSubsystem.h"
#include "Components/StaticMeshComponent.h"

AItemActor* UPalItemSpawnSubsystem::SpawnItemsFromMesh(UWorld* World, FName ItemName, FTransform Origin, AActor* Owner )
{
	FActorSpawnParameters SpawnParam{};
	SpawnParam.Owner = Owner;
	return nullptr;
}
