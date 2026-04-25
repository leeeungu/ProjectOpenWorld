#pragma once

#include "CoreMinimal.h"
#include "FoliageInstancedStaticMeshComponent.h"
#include "Interaction/InteractionInterface.h"
#include "PalFoliageInstanceComponent.generated.h"

class UItemSpawnListAssetUserData;

UCLASS()
class PROJECTOPENWORLD_API UPalFoliageInstanceComponent : public UFoliageInstancedStaticMeshComponent
	, public IInteractionInterface // PAL 작업 때문에 상속됨
{
	GENERATED_BODY()

protected:
	// Indices 순서에 변경 가능성이 있어서
	TMap<FVector, int32> ItemSpawnMap{};

	TWeakObjectPtr<UItemSpawnListAssetUserData> ItemSpawnListAssetUserDataPtr{};
	TQueue< FTransform> PendingSpawnItemQueue{};
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
