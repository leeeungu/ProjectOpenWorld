#include "Item/Notify/AnimNotify_ItemSpawnFromFoliage.h"
#include "DrawDebugHelpers.h"
#include "Pal/Interface/PalHarvestable.h"

void UAnimNotify_ItemSpawnFromFoliage::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	FVector Center = MeshComp->GetSocketLocation(ShearchSocketName) + MeshComp->GetComponentRotation().Quaternion() * ShearchOffset;
	UWorld* pWorld = MeshComp->GetWorld();
	if (pWorld)
	{
		FCollisionQueryParams Param{};
		FCollisionShape ShearchShape = FCollisionShape::MakeSphere(ShearchRadius);
		TArray<FHitResult> arHitResult{};
		TSet<UActorComponent*> ProcessedFoliageComps{};

		pWorld->SweepMultiByChannel(arHitResult, Center, Center, FQuat::Identity,ECollisionChannel::ECC_WorldStatic, ShearchShape, Param);
		for (const FHitResult& Hit : arHitResult) //Hit.Item 가 UPalFoliageInstanceComponent에 충돌된 instance의 index도 알려줌
		{
			TScriptInterface<IPalHarvestable> Harvestable = Hit.GetComponent();
			if (Harvestable && !ProcessedFoliageComps.Find(Hit.GetComponent())) // bool operator 호출
			{
				ProcessedFoliageComps.Add(Hit.GetComponent());
				Harvestable->OnHarvestEvent(FHarvestEventData{ MeshComp->GetOwner(),&Hit });
			}

			//UPalFoliageInstanceComponent* FoliageComp = Cast<UPalFoliageInstanceComponent>(Hit.GetComponent());
			//if (FoliageComp && !ProcessedFoliageComps.Find(FoliageComp))
			//{
			//	ProcessedFoliageComps.Add(FoliageComp);
			//	TArray<int32> InstanceIndexArray = FoliageComp->GetInstancesOverlappingSphere(Center, ShearchRadius);
			//	FoliageComp->RemoveInstances(FoliageComp->SpawnItem(InstanceIndexArray));
			//}
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
