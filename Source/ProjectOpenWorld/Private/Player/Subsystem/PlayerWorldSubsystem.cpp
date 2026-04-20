#include "Player/Subsystem/PlayerWorldSubsystem.h"
#include "Player/Character/PlayerPreviewPawn.h"

void UPlayerWorldSubsystem::RegisterPreviewActor(APlayerPreviewPawn* Actor)
{
	PlayerPreviewActor = Actor;
}

APlayerPreviewPawn* UPlayerWorldSubsystem::GetPreviewActor() const
{
	return PlayerPreviewActor.Get();
}

