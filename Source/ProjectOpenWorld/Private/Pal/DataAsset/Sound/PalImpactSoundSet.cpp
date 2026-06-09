#include "Pal/DataAsset/Sound/PalImpactSoundSet.h"

USoundBase* UPalImpactSoundSet::GetSoundData(EPhysicalSurface Type) const
{
    if (const TObjectPtr<USoundBase>* Found = SurfaceSounds.Find(Type))
        return *Found;
    return DefaultSound;
}
