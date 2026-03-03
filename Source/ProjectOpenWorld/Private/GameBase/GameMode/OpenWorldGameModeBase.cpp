#include "GameBase/GameMode/OpenWorldGameModeBase.h"

void AOpenWorldGameModeBase::StartPlay()
{
	Super::StartPlay();
	USoundGameInstanceSubsystem::PlayMainBGMSound(StartBGMType);
}
