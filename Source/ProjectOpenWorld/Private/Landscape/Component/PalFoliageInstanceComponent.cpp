#include "Landscape/Component/PalFoliageInstanceComponent.h"
#include "Item/AssetUserData/ItemSpawnListAssetUserData.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Item/Actor/ItemActor.h"
#include "Resource/Interface/ResourceInterface.h"
#include "PhysicsEngine/BodySetup.h"

void UPalFoliageInstanceComponent::ResetItemSpawnMap()
{
	if (PendingSpawnItemQueue.IsEmpty() || !GetStaticMesh())
		return;
	FTransform Instance{};
	PendingSpawnItemQueue.Dequeue(Instance);

	bool bCanAddInstance = true;
	TArray<FHitResult > arHitResult{};

	const FKAggregateGeom& AggGeom = GetStaticMesh()->GetBodySetup()->AggGeom;
	// box check
	for (int32 i = 0; i < AggGeom.GetElementCount(EAggCollisionShape::Box) && bCanAddInstance; i++)
	{
		// boxtrace
		FKBoxElem BoxElem = AggGeom.BoxElems[i];
		FVector BoxCenter = Instance.TransformPosition(BoxElem.Center);
		FQuat BoxRotation = Instance.GetRotation() * BoxElem.Rotation.Quaternion();
		FVector BoxExtent = FVector(BoxElem.X, BoxElem.Y, BoxElem.Z);
		GetWorld()->SweepMultiByChannel(
			arHitResult,
			BoxCenter,
			BoxCenter,
			BoxRotation,
			ECollisionChannel::ECC_Visibility,
			FCollisionShape::MakeBox(BoxExtent),
			FCollisionQueryParams::DefaultQueryParam
		);
		DrawDebugBox(GetWorld(), BoxCenter, BoxExtent, BoxRotation, FColor::Green, false, 10.0f);

		for (const FHitResult& Hit : arHitResult)
		{
			if (Hit.GetActor() && !Hit.GetActor()->ActorHasTag("Landscape") && Hit.GetActor() != GetOwner())
			{
				bCanAddInstance = false;
				break;
			}
		}

	}
	// capsule CHECK
	for (int32 i = 0; i < AggGeom.GetElementCount(EAggCollisionShape::Sphyl) && bCanAddInstance; i++)
	{
		FKSphylElem SphylElem = AggGeom.SphylElems[i];
		FVector CapsuleCenter = Instance.TransformPosition(SphylElem.Center);
		FQuat CapsuleRotation = Instance.GetRotation() * SphylElem.Rotation.Quaternion();
		FVector CapsuleAxis = CapsuleRotation.GetUpVector();
		FVector Start = CapsuleCenter + CapsuleAxis * SphylElem.Length * 0.5f;
		FVector End = CapsuleCenter - CapsuleAxis * SphylElem.Length * 0.5f;
		GetWorld()->SweepMultiByChannel(
			arHitResult,
			Start,
			End,
			FQuat::Identity,
			ECollisionChannel::ECC_Visibility,
			FCollisionShape::MakeCapsule(SphylElem.Radius, SphylElem.Length * 0.5f),
			FCollisionQueryParams::DefaultQueryParam
		);
		DrawDebugCapsule(GetWorld(), CapsuleCenter, SphylElem.Length * 0.5f, SphylElem.Radius, CapsuleRotation, FColor::Blue, false, 10.0f);
		for (const FHitResult& Hit : arHitResult)
		{
			if (Hit.GetActor() && !Hit.GetActor()->ActorHasTag("Landscape") && Hit.GetActor() != GetOwner())
			{
				bCanAddInstance = false;
				break;
			}
		}
	}
	// sphere check
	for (int32 i = 0; i < AggGeom.GetElementCount(EAggCollisionShape::Sphere) && bCanAddInstance; i++)
	{
		FKSphereElem SphereElem = AggGeom.SphereElems[i];
		FVector SphereCenter = Instance.TransformPosition(SphereElem.Center);
		GetWorld()->SweepMultiByChannel(
			arHitResult,
			SphereCenter,
			SphereCenter,
			FQuat::Identity,
			ECollisionChannel::ECC_Visibility,
			FCollisionShape::MakeSphere(SphereElem.Radius),
			FCollisionQueryParams::DefaultQueryParam
		);
		DrawDebugSphere(GetWorld(), SphereCenter, SphereElem.Radius, 12, FColor::Red, false, 10.0f);
		for (const FHitResult& Hit : arHitResult)
		{
			if (Hit.GetActor() && !Hit.GetActor()->ActorHasTag("Landscape") && Hit.GetActor() != GetOwner())
			{
				bCanAddInstance = false;
				break;
			}
		}
	}

	if (bCanAddInstance)
	{
		AddInstance(Instance, true);
	}
}

TArray<int32> UPalFoliageInstanceComponent::SpawnItem(const TArray<int32>& InstanceIndices)
{
	TArray<int32> RemoveIndices{};
	if (ItemSpawnListAssetUserDataPtr.IsValid())
	{
		FActorSpawnParameters SpawnParam{};
		SpawnParam.Owner = GetOwner();
		SpawnParam.bNoFail = true;

		for (int i = 0; i < InstanceIndices.Num(); i++)
		{
			FItemSpawnRateData SpawnData = ItemSpawnListAssetUserDataPtr->GetRandomItem();
			FName ItemName = SpawnData.ItemName;

			FTransform InstanceTransform{};
			GetInstanceTransform(InstanceIndices[i], InstanceTransform,  true);
			int32& Count = ItemSpawnMap.FindOrAdd(InstanceTransform.GetLocation(), ItemSpawnListAssetUserDataPtr->GetMaxSpawnCount());
			Count--;
			TSubclassOf<AItemActor> SpawnClass = UItemDataSubsystem::GetPalStaticItemVisualBlueprintClassSoftByName(ItemName);
			UE_LOG(LogTemp, Warning, TEXT("UPalFoliageInstanceComponent::SpawnItem : Spawning Item %s at Location %s, Remaining Count %d"),
				*ItemName.ToString(),
				*InstanceTransform.GetLocation().ToString(),
				Count);
			if (SpawnClass)
			{
				FVector NewLocation = InstanceTransform.GetLocation();
				NewLocation += FVector(0, 0, 200); // Spawn above ground
				AActor* SpawnedActor = GetWorld()->SpawnActor
					(
						SpawnClass.Get(), 
						&NewLocation, {}, SpawnParam
					);
				if (SpawnedActor)
				{
					AItemActor* ItemActor = Cast<AItemActor>(SpawnedActor);
					ItemActor->Init(ItemName, SpawnData.SpawnRandomCount);
				}
			}
			if (Count <= 0)
			{
				Count = 0;
				ItemSpawnMap.Remove(InstanceTransform.GetLocation());
				RemoveIndices.Add(InstanceIndices[i]);
				PendingSpawnItemQueue.Enqueue(InstanceTransform);
				FTimerHandle TimerHandle{};
				GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle,
					this, 
					& UPalFoliageInstanceComponent::ResetItemSpawnMap,
					15.0f,
					false);

				///FTimerManager::get::SetTimer()
			}
		}
	}
	return RemoveIndices;
}

bool UPalFoliageInstanceComponent::SetStaticMesh(UStaticMesh* NewMesh)
{
	if (NewMesh)
	{
		ItemSpawnListAssetUserDataPtr = MakeWeakObjectPtr
		(
			Cast< UItemSpawnListAssetUserData>
			(
				NewMesh->GetAssetUserDataOfClass(UItemSpawnListAssetUserData::StaticClass())
			)
		);
	}
	else
	{
		ItemSpawnListAssetUserDataPtr = nullptr;
	}
	return Super::SetStaticMesh(NewMesh);
}

void UPalFoliageInstanceComponent::ClearInstances()
{
	ItemSpawnMap.Empty();
	PendingSpawnItemQueue.Empty();
	Super::ClearInstances();
}

void UPalFoliageInstanceComponent::OnBeginDetected_Implementation(ACharacter* pOther)
{
	UE_LOG(LogTemp, Log, TEXT("UPalFoliageInstanceComponent :: OnBeginDetected_Implementation"));
}

void UPalFoliageInstanceComponent::OnEndDetected_Implementation(ACharacter* pOther)
{
	UE_LOG(LogTemp, Log, TEXT("UPalFoliageInstanceComponent :: OnEndDetected_Implementation"));
}

void UPalFoliageInstanceComponent::OnInteractionStart_Implementation(ACharacter* pOther)
{
	if(pOther && pOther->Implements<UResourceInterface>())
	IResourceInterface::Execute_StartResource(pOther, nullptr);
}

void UPalFoliageInstanceComponent::OnInteraction_Implementation(ACharacter* pOther)
{

	if (pOther && pOther->Implements<UResourceInterface>())
	IResourceInterface::Execute_StopResource(pOther, nullptr);
}

void UPalFoliageInstanceComponent::OnInteractionEnd_Implementation(ACharacter* pOther)
{
	if (pOther && pOther->Implements<UResourceInterface>())
	IResourceInterface::Execute_StopResource(pOther, nullptr);
}
