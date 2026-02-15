#include "GameBase/GameMode/OpenWorldGameModeBase.h"
#include "GameBase/Subsystem/SoundGameInstanceSubsystem.h"

void AOpenWorldGameModeBase::StartPlay()
{
	Super::StartPlay();
	USoundGameInstanceSubsystem::PlayMainBGMSound(EBGMSoundType::BGMST_InGame);
}
