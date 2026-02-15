#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "ResourceEndEvent.generated.h"


class UMeshComponent;
enum EEffectSoundType : uint8;

UCLASS()
class PROJECTOPENWORLD_API UResourceEndEvent : public UAssetUserData
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource")
	float MinReSpawnTimeSec = 15.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource")
	float MaxReSpawnTimeSec = 15.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource")
	TEnumAsByte < EEffectSoundType> ResourceEndSound{};

public:
	float GetRandomReSpawnTime() const { return FMath::FRandRange(MinReSpawnTimeSec, MaxReSpawnTimeSec); }
	virtual void ResourceEndEvent(UMeshComponent* OwnerComponent, FTransform MeshTransform);
};
