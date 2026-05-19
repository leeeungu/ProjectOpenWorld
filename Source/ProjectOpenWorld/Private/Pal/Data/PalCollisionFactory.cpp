#include "Pal/Data/PalCollisionFactory.h"
#include "Engine/EngineTypes.h"

ECollisionChannel PalDamage::GetDamageCollisionChannel()
{
	return  ECollisionChannel::ECC_GameTraceChannel4;
}

ECollisionChannel PalJob::GetHarvestCollisionChannel()
{
	return ECollisionChannel::ECC_GameTraceChannel5;
}
