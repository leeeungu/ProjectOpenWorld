#include "Item/Notify/AnimNotify_ItemSpawnFromFoliage.h"
#include "FoliageInstancedStaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Landscape/Component/PalFoliageInstanceComponent.h"

void UAnimNotify_ItemSpawnFromFoliage::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	UWorld* pWorld = MeshComp->GetWorld();
	if (pWorld)
	{
		FCollisionQueryParams Param{};
		FCollisionResponseParams ResponseParam{};
		FCollisionShape ShearchShape = FCollisionShape::MakeSphere(ShearchRadius);
		FCollisionObjectQueryParams ObjectQueryParam{};
		TArray<FHitResult> arHitResult{};

		pWorld->SweepMultiByChannel(arHitResult,
			MeshComp->GetSocketLocation(ShearchSocketName) + MeshComp->GetComponentRotation().Quaternion() * ShearchOffset,
			MeshComp->GetSocketLocation(ShearchSocketName) + MeshComp->GetComponentRotation().Quaternion() * ShearchOffset,
			FQuat::Identity,
			ECollisionChannel::ECC_WorldStatic,
			ShearchShape,
			Param);
		TSet<TSoftObjectPtr<UPalFoliageInstanceComponent>> ProcessedFoliageComps{};
		for (const FHitResult& Hit : arHitResult)
		{
			UPalFoliageInstanceComponent* FoliageComp = Cast<UPalFoliageInstanceComponent>(Hit.GetComponent());
			if (FoliageComp && !ProcessedFoliageComps.Find(FoliageComp))
			{
				ProcessedFoliageComps.Add(FoliageComp);
				TArray<int32> InstanceIndexArray = FoliageComp->GetInstancesOverlappingSphere(
						MeshComp->GetSocketLocation(ShearchSocketName) + MeshComp->GetComponentRotation().Quaternion() * ShearchOffset,
						ShearchRadius);
				FoliageComp->RemoveInstances(FoliageComp->SpawnItem(InstanceIndexArray));
			}
		}
	}

#if WITH_EDITOR	
	if (pWorld  && !pWorld->HasBegunPlay())
	{
		FVector Start = MeshComp->GetSocketLocation(ShearchSocketName) + MeshComp->GetComponentRotation().Quaternion() * ShearchOffset;
		FVector End = Start;
		DrawDebugSphere(pWorld, Start, ShearchRadius, 12, FColor::Red, false, 0.5f);
	}
#endif 
}
