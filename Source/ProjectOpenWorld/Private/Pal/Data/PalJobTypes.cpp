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
    case EPalJobType::Lumbering:    return EPalWorkCapability::Lumbering;
    case EPalJobType::Transport:    return EPalWorkCapability::Transport;
    case EPalJobType::Attack:    return EPalWorkCapability::Attack;
    default:                        return (EPalWorkCapability)0;
    }
}
