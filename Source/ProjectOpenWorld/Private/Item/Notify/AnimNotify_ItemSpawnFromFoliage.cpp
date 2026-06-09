#include "Item/Notify/AnimNotify_ItemSpawnFromFoliage.h"
#include "DrawDebugHelpers.h"
#include "Pal/Interface/PalHarvestable.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "Pal/DataAsset/Sound/PalImpactSoundSet.h"

void UAnimNotify_ItemSpawnFromFoliage::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	FVector Center = MeshComp->GetSocketLocation(ShearchSocketName) + MeshComp->GetComponentRotation().Quaternion() * ShearchOffset;
	UWorld* pWorld = MeshComp->GetWorld();
	AActor* Owner = MeshComp->GetOwner();
	if (pWorld && Owner)
	{
		FCollisionQueryParams Param{};
		Param.bReturnPhysicalMaterial = true;
		FCollisionShape ShearchShape = FCollisionShape::MakeSphere(ShearchRadius);
		TArray<FHitResult> arHitResult{};
		TSet<UActorComponent*> ProcessedFoliageComps{};

		pWorld->SweepMultiByChannel(arHitResult, Center, Center, FQuat::Identity,ECollisionChannel::ECC_WorldStatic, ShearchShape, Param);
		for (const FHitResult& Hit : arHitResult) 
		{
			if (Hit.GetActor() != Owner)
			{
				TScriptInterface<IPalHarvestable> Harvestable = Hit.GetComponent();
				if (Harvestable && !ProcessedFoliageComps.Find(Hit.GetComponent())) // bool operator 호출
				{
					if (Hit.PhysMaterial.IsValid() && SoundSet)
					{
						UGameplayStatics::SpawnSoundAtLocation(pWorld, SoundSet->GetSoundData(Hit.PhysMaterial->SurfaceType), Hit.Location);
					}
					ProcessedFoliageComps.Add(Hit.GetComponent());
					//Hit.Item 가 UPalFoliageInstanceComponent에 충돌된 instance의 index도 알려줌
					Harvestable->OnHarvestEvent(FHarvestEventData{ Owner,&Hit });
				}
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
