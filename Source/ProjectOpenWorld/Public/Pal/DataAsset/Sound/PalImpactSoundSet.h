#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PalImpactSoundSet.generated.h"

enum EPhysicalSurface : int;
class USoundBase;

UCLASS()
class PROJECTOPENWORLD_API UPalImpactSoundSet : public UDataAsset
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, Category = "ImpactSound")
	TMap< TEnumAsByte<EPhysicalSurface>, TObjectPtr<USoundBase>> SurfaceSounds{};

	UPROPERTY(EditAnywhere, Category = "ImpactSound")
	TObjectPtr<USoundBase> DefaultSound{};   // 미등록 표면 폴백
public:
	USoundBase* GetSoundData(EPhysicalSurface Type) const;
	
};
