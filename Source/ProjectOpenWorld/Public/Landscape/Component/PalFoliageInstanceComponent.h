#pragma once

#include "CoreMinimal.h"
#include "FoliageInstancedStaticMeshComponent.h"
#include "Interaction/InteractionInterface.h"
#include "PalFoliageInstanceComponent.generated.h"

class UItemSpawnListAssetUserData;

UCLASS()
class PROJECTOPENWORLD_API UPalFoliageInstanceComponent : public UFoliageInstancedStaticMeshComponent
	, public IInteractionInterface
{
	GENERATED_BODY()

protected:
	TMap<FVector, int32> ItemSpawnMap{};

	TWeakObjectPtr<UItemSpawnListAssetUserData> ItemSpawnListAssetUserDataPtr{};
	TQueue< FTransform> PendingSpawnItemQueue{};
		//GetStaticMesh()->GetAssetUserDataOfClass(UItemSpawnListAssetUserData::StaticClass());

	void ResetItemSpawnMap();
public:
	TArray<int32> SpawnItem(const TArray<int32>& InstanceIndices);
	
	virtual bool SetStaticMesh(class UStaticMesh* NewMesh) override;
	virtual void ClearInstances() override;

	// IInteractionInterface
	virtual void OnBeginDetected_Implementation(ACharacter* pOther) override;
	virtual void OnEndDetected_Implementation(ACharacter* pOther) override;
	virtual void OnInteractionStart_Implementation(ACharacter* pOther) override;
	virtual void OnInteraction_Implementation(ACharacter* pOther) override;
	virtual void OnInteractionEnd_Implementation(ACharacter* pOther) override;
	virtual void OnInteractionCanceled_Implementation(ACharacter* pOther) override {}
};
