#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "HarvestableAssetUserData.generated.h"

class USoundBase;

UCLASS()
class PROJECTOPENWORLD_API UHarvestableAssetUserData : public UAssetUserData
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HarvestableData")
	TObjectPtr<USoundBase> HitSound{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HarvestableData")
	float HitVolumeMultiplier = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HarvestableData")
	float HitPitchMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HarvestableData")
	TObjectPtr<USoundBase> DestroySound{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HarvestableData")
	float DestroyVolumeMultiplier = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HarvestableData")
	float DestroyPitchMultiplier = 1.f;

public:
	USoundBase* GetHitSound() const { return HitSound; }
	USoundBase* GetDestroySound() const { return DestroySound; }

	void PlayHarvestHitSound(UMeshComponent* OwnerComponent, FTransform MeshTransform) const;
	void PlayHarvestDestroySound(UMeshComponent* OwnerComponent, FTransform MeshTransform) const;
};
