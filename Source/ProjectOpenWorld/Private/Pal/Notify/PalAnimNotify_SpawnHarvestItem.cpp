#include "Pal/Notify/PalAnimNotify_SpawnHarvestItem.h"
#include "FoliageInstancedStaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Landscape/Component/PalFoliageInstanceComponent.h"
#include "Pal/Data/PalCollisionFactory.h"
#include "Pal/Interface/PalHarvestable.h"

void UPalAnimNotify_SpawnHarvestItem::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
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

		pWorld->SweepMultiByChannel(arHitResult, Center, Center, FQuat::Identity, PalJob::GetHarvestCollisionChannel(), ShearchShape, Param);
		for (const FHitResult& Hit : arHitResult)
		{
			TScriptInterface<IPalHarvestable> Harvestable = Hit.GetComponent();
			if (Harvestable && !ProcessedFoliageComps.Find(Hit.GetComponent())) // bool operator 호출
			{
				ProcessedFoliageComps.Add(Hit.GetComponent());
				Harvestable->OnHarvestEvent(FHarvestEventData{ MeshComp->GetOwner(),&Hit});
			}
		}
	}

#if WITH_EDITOR	
	if (pWorld && !pWorld->HasBegunPlay())
	{
		FVector Start = Center;
		FVector End = Start;
		DrawDebugSphere(pWorld, Start, ShearchRadius, 12, FColor::Red, false, 0.5f);
	}
#endif 
}
