#include "Pal/Data/PalJobTypes.h"
#include "Pal/Interface/PalWorkable.h"
#include "Engine/EngineTypes.h"

bool FPalWorkCommand::IsValidWork() const
{
	if (pTarget.IsValid())
	{
		if (const IPalWorkable* Workable = Cast<IPalWorkable>(pTarget.Get()))
		{
			return Workable->IsWorkable();
		}
	}
	return false;
}


EPalWorkCapability PalJobUtils::ToCapability(EPalJobType Type)
{
    switch (Type)
    {
    case EPalJobType::Architecture: return EPalWorkCapability::Architecture;
    case EPalJobType::Mining:       return EPalWorkCapability::Mining;
    case EPalJobType::Deforest:    return EPalWorkCapability::Deforest;
    case EPalJobType::Transport:    return EPalWorkCapability::Transport;
    case EPalJobType::Cooking:    return EPalWorkCapability::Cooking;
    case EPalJobType::Planting:    return EPalWorkCapability::Planting;
    case EPalJobType::Attack:    return EPalWorkCapability::Attack;
    case EPalJobType::CommonWork:    return EPalWorkCapability::CommonWork;
    default:                        return (EPalWorkCapability)0;
    }
}