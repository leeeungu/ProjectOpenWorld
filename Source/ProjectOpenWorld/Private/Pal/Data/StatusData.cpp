#include "Pal/Data/StatusData.h"
#include "Pal/Data/PalJobTypes.h"

EStatusType PalStatus::GetJobWorkSpeedStatus(EPalJobType JobType)
{
	switch (JobType)
	{
	case EPalJobType::Architecture:
		return EStatusType::Architecture;
		break;
	case EPalJobType::Mining:
		return EStatusType::Mining;
		break;
	case EPalJobType::Deforest:
		return EStatusType::Deforest;
		break;
	case EPalJobType::Transport:
		return EStatusType::TransportSpeed;
		break;
	case EPalJobType::Attack:
		return EStatusType::Attack;
		break;
	}
	return EStatusType::None;
}
