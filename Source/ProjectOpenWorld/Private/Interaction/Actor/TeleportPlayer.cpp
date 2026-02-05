#include "Interaction/Actor/TeleportPlayer.h"
#include "Engine/TargetPoint.h"

ATeleportPlayer::ATeleportPlayer()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATeleportPlayer::BeginPlay()
{
	Super::BeginPlay();
}

//void ATeleportPlayer::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//}

