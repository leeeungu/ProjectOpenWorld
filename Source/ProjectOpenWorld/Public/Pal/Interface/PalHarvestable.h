#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PalHarvestable.generated.h"

USTRUCT(BlueprintType)
struct FHarvestEventData
{
	GENERATED_USTRUCT_BODY()
public:
	FHarvestEventData() = default;
	FHarvestEventData(AActor* Harvester, const FHitResult* HitInfo)
	{
		this->Harvester = Harvester;
		Hit = HitInfo;
	}

	UPROPERTY(VisibleAnywhere)
	AActor* Harvester{};
	const FHitResult* Hit{};
	UPROPERTY(VisibleAnywhere)
	int HarvestCount = 1;
};

UINTERFACE(MinimalAPI)
class UPalHarvestable : public UInterface
{
	GENERATED_BODY()
};

// StaticMeshComponent 자손에 붙이는 interface
class PROJECTOPENWORLD_API IPalHarvestable
{
	GENERATED_BODY()
public:
	virtual void OnHarvestEvent(FHarvestEventData EventData) = 0;
};
