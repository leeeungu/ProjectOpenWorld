#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "ResourceEndEvent.generated.h"


class UMeshComponent;

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
	TObjectPtr<USoundBase> ResourceEndSound = nullptr;

public:
	float GetRandomReSpawnTime() const { return FMath::FRandRange(MinReSpawnTimeSec, MaxReSpawnTimeSec); }
	virtual void ResourceEndEvent(UMeshComponent* OwnerComponent, FTransform MeshTransform);
};
