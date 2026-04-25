#include "Item/Notify/AnimNotify_ItemSpawnFromFoliage.h"
#include "FoliageInstancedStaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Landscape/Component/PalFoliageInstanceComponent.h"

void UAnimNotify_ItemSpawnFromFoliage::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	FVector Center = MeshComp->GetSocketLocation(ShearchSocketName) + MeshComp->GetComponentRotation().Quaternion() * ShearchOffset;
	UWorld* pWorld = MeshComp->GetWorld();
	if (pWorld)
	{
		FCollisionQueryParams Param{};
		FCollisionResponseParams ResponseParam{};
		FCollisionShape ShearchShape = FCollisionShape::MakeSphere(ShearchRadius);
		FCollisionObjectQueryParams ObjectQueryParam{};
		TArray<FHitResult> arHitResult{};
		TSet<UPalFoliageInstanceComponent*> ProcessedFoliageComps{};

		pWorld->SweepMultiByChannel(arHitResult, Center, Center, FQuat::Identity,ECollisionChannel::ECC_WorldStatic, ShearchShape, Param);
		for (const FHitResult& Hit : arHitResult)
		{
			UPalFoliageInstanceComponent* FoliageComp = Cast<UPalFoliageInstanceComponent>(Hit.GetComponent());
			if (FoliageComp && !ProcessedFoliageComps.Find(FoliageComp))
			{
				ProcessedFoliageComps.Add(FoliageComp);
				TArray<int32> InstanceIndexArray = FoliageComp->GetInstancesOverlappingSphere(Center, ShearchRadius);
				FoliageComp->RemoveInstances(FoliageComp->SpawnItem(InstanceIndexArray));
			}
		}
	}

#if WITH_EDITOR	
	if (pWorld  && !pWorld->HasBegunPlay())
	{
		FVector Start = Center;
		FVector End = Start;
		DrawDebugSphere(pWorld, Start, ShearchRadius, 12, FColor::Red, false, 0.5f);
	}
#endif 
}
