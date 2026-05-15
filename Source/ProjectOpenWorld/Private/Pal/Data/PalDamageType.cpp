#include "Pal/Data/PalDamageType.h"
#include "Engine/EngineTypes.h"

ECollisionChannel PalDamage::GetDamageCollisionChannel()
{
	return  ECollisionChannel::ECC_GameTraceChannel4;
}
