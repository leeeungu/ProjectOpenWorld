#include "Pal/Data/PalJobTypes.h"
#include "Pal/Interface/PalWorkable.h"

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


